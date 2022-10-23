#include "cc/xcrypto.h"
#include "cmd/io_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "gmssl/hex.h"
#include "gmssl/sm3.h"

static void sm4_before(cmdp_before_param_st *params);
static cmdp_action_t sm4_process(cmdp_process_param_st *params);

static struct
{
    bool decrypt;
    char *phrase;
    char *key;
    char *iv;
    char *informat;
    char *outformat;
    char *infile;
    char *outfile;
} sm4_args;

cmdp_command_st sc_sm4 = {
    .name = "sm4",
    .desc = "Encrypt or decrypt with SM4",
    // .doc  = "Usage: sm4 [-d] [-p | -k,--iv] [infile [outfile]]\n",
    .options =
        (cmdp_option_st[]){
            {'d', "decrypt", "decrypt", CMDP_TYPE_BOOL, &sm4_args.decrypt},
            {'p', "phrase", "phrase to generate key and iv", CMDP_TYPE_STRING_PTR, &sm4_args.phrase},
            {'k', "key", "key (16 bytes)", CMDP_TYPE_STRING_PTR, &sm4_args.key, "<hex>"},
            {0, "iv", "IV (16 bytes)", CMDP_TYPE_STRING_PTR, &sm4_args.iv, "<hex>"},
            {'I', "in-fmt", "Input Format", CMDP_TYPE_STRING_PTR, &sm4_args.informat, "<FORMAT>"},
            {'O', "out-fmt", "Output Format", CMDP_TYPE_STRING_PTR, &sm4_args.outformat, "<FORMAT>"},
            {'i', "infile", "Input File", CMDP_TYPE_STRING_PTR, &sm4_args.infile, "<FILE>"},
            {'o', "outfile", "Output File", CMDP_TYPE_STRING_PTR, &sm4_args.outfile, "<FILE>"},
            {0},
        },
    .fn_before  = sm4_before,
    .fn_process = sm4_process,
};


static void sm4_before(cmdp_before_param_st *params)
{
    memset(&sm4_args, 0, sizeof(sm4_args));
}
static cmdp_action_t sm4_process(cmdp_process_param_st *params)
{
    // char *prog      = NULL;
    uint8_t key[16] = {0};
    uint8_t iv[16]  = {0};
    XIO *instream   = NULL;
    XIO *outstream  = NULL;
    int ret         = X_FAILURE;

    if (params->argc == 0 && params->opts == 0)
    {
        return CMDP_ACT_SHOW_HELP;
    }
    if (params->argc > 1)
    {
        LOG_C(0, "too many arguments");
        goto end;
    }
    if (sm4_args.phrase && (sm4_args.key || sm4_args.iv))
    {
        fprintf(stderr, "Can't use --phrase and --key/--iv together.");
        return CMDP_ACT_FAIL;
    }
    if (sm4_args.phrase)
    {
        SM3_CTX ctx;
        uint8_t dgst[32];
        sm3_init(&ctx);
        sm3_update(&ctx, sm4_args.phrase, strlen(sm4_args.phrase));
        sm3_finish(&ctx, dgst);
        memcpy(key, dgst, 16);
        memcpy(iv, dgst + 16, 16);
    }
    else
    {
        hex_to_bytes(sm4_args.key, strlen(sm4_args.key), key, NULL);
        hex_to_bytes(sm4_args.iv, strlen(sm4_args.iv), iv, NULL);
    }
    if (params->argc > 0 && sm4_args.infile)
    {
        LOG_C(0, "Can't use argument text and input file together.");
        goto end;
    }

    XIO_CMD_IN_PARAM in_param = {
        .filename     = sm4_args.infile,
        .file_deffmt  = "bin",
        .argument     = params->argc > 0 ? params->argv[0] : NULL,
        .arg_deffmt   = sm4_args.decrypt ? "hex" : "bin",
        .stdin_deffmt = sm4_args.decrypt ? "hex" : "bin",
        .format       = sm4_args.informat,
    };
    instream = XIO_new_cmd_instream(&in_param);

    XIO_CMD_OUT_PARAM out_param = {
        .filename      = sm4_args.outfile,
        .file_deffmt   = "bin",
        .stdout_deffmt = sm4_args.decrypt ? "bin" : "hex",
        .format        = sm4_args.outformat,
    };
    outstream = XIO_new_cmd_outstream(&out_param);

    if (sm4_args.decrypt)
    {
        ret = x_sm4_cbc_decrypt(key, iv, instream, outstream);
    }
    else
    {
        ret = x_sm4_cbc_encrypt(key, iv, instream, outstream);
    }
    ret = X_SUCCESS;

end:
    XIO_close(instream);
    XIO_close(outstream);
    return ret == X_SUCCESS ? CMDP_ACT_OVER : CMDP_ACT_FAIL;
}
