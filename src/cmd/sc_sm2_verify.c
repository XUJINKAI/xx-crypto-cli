#include "cc/format/hex.h"
#include "cc/sm2.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "utils/converter.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *sig;
    char *key;
    char *pass;
    char *id;
    char *instring;
    char *infile;
    char *bufsize;
    char *fail_code;
} args;

cmdp_command_st sc_sm2_verify = {
    .name = "verify",
    .desc = "SM2 signature verification",
    .doc  = "Usage: sm2 verify [OPTIONS] <TEXT>\n",
    .options =
        (cmdp_option_st[]){
            {0, "sig", "Signature", CMDP_TYPE_STRING_PTR, &args.sig, "<SIG>"},
            {0, "key", "Raw public key HEX or PEM file", CMDP_TYPE_STRING_PTR, &args.key, "<KEY>"},
            {0, "pass", "Password for the private key", CMDP_TYPE_STRING_PTR, &args.pass, "<PASS>"},
            {0, "id", "Signature id", CMDP_TYPE_STRING_PTR, &args.id, "<ID>"},
            _opt_intext(args.instring, ),
            _opt_infile(args.infile, ),
            {0, "bufsize", "Update buffer size", CMDP_TYPE_STRING_PTR, &args.bufsize, "<size>"},
            {0, "fail-code", "Verify fail code, default: 0", CMDP_TYPE_STRING_PTR, &args.fail_code, "<code>"},
            {0},
        },
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_EMPTY_HELP(params);
    CMDP_CHECK_ARG_COUNT(params, 0, 0);

    RESULT ret                          = RET_FAIL;
    XIO *instream                       = NULL;
    XIO *outstream                      = NULL;
    SM2_KEY key                         = {0};
    char *id                            = args.id ? args.id : SM2_DEFAULT_ID;
    long bufsize                        = 0;
    uint8_t sig[SM2_MAX_SIGNATURE_SIZE] = {0};
    size_t siglen                       = 0;
    long fail_code                      = 0;

    instream  = cmd_get_instream(args.instring, args.infile, true);
    outstream = cmd_get_outstream(NULL, true);

    if (RET_OK != cc_sm2_try_load_public_key(&key, args.key, strlen(args.key), args.pass))
    {
        LOG_ERROR("Failed to load private key");
        goto end;
    }
    if (RET_OK != cc_hex_dec_max_len(args.sig, sizeof(sig), sig, &siglen))
    {
        LOG_ERROR("Failed to load signature");
        goto end;
    }
    if (RET_OK != string_to_long(args.bufsize, &bufsize, CONV_SIZE | CONV_NULL_OK))
    {
        LOG_ERROR("Invalid buffer size: %s", args.bufsize);
        goto end;
    }
    if (RET_OK != string_to_long(args.fail_code, &fail_code, CONV_NULL_OK))
    {
        LOG_ERROR("Invalid fail code: %s", args.fail_code);
        goto end;
    }

    bool result;
    ret = cc_sm2_verify(instream, &key, id, strlen(id), bufsize, sig, siglen, &result);
    if (RET_OK != ret)
    {
        LOG_ERROR("Failed to verify signature");
        goto end;
    }

    XIO_printf(outstream, "Verify: %s", result ? "success" : "failed");

    ret = RET_OK;
    if (args.fail_code && !result)
    {
        ret                = RET_FAIL;
        params->error_code = (int)fail_code;
    }

end:
    xio_close_safe(instream);
    xio_close_safe(outstream);
    clear_buffer(&key, sizeof(key));

    return RESULT_TO_CMDP_ACTION(ret);
}
