#include "cc/cipher.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "gmssl/hex.h"
#include "gmssl/pbkdf2.h"
#include "gmssl/sm3.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    bool decrypt;
    char *mode;
    char *key;
    char *iv;
    char *phrase;
    char *text;
    char *infile;
    char *inform;
    char *outfile;
    char *outform;
    bool hex;
} __args;

cmdp_command_st sc_sm4 = {
    .name = "sm4",
    .desc = "SM4 Symmetric Algorithm",
    .doc  = "Usage: sm4 [-d] [-p | --key,--iv] [OPTIONS]\n",
    .options =
        (cmdp_option_st[]){
            CMDP_OPT_DOC("\nGeneral options:\n"),
            {'d', "decrypt", "decrypt", CMDP_TYPE_BOOL, &__args.decrypt},
            {0, "mode", "Mode: cbc(default), ctr", CMDP_TYPE_STRING_PTR, &__args.mode, "<MODE>"},
            {0, "key", "Raw key, in hex (16 bytes)", CMDP_TYPE_STRING_PTR, &__args.key, "<hex>"},
            {0, "iv", "IV, in hex (16 bytes)", CMDP_TYPE_STRING_PTR, &__args.iv, "<hex>"},
            {'p', "phrase", "Passphrase to generate key and iv", CMDP_TYPE_STRING_PTR, &__args.phrase},
            CMDP_OPT_DOC("\nInput options:\n"),
            _opt_intext(__args.text, ),
            _opt_infile(__args.infile, ),
            _opt_informat(__args.inform, ),
            CMDP_OPT_DOC("\nOutput options:\n"),
            _opt_outfile(__args.outfile, ),
            _opt_outformat(__args.outform, ),
            {0, "hex", "Shortcut for --outform hex", CMDP_TYPE_BOOL, &__args.hex},
            {0},
        },
    .doc_tail = "\n"
                "Global options also available: --verbose, --debug, etc.\n"
                "FORMAT: bin, hex.\n",
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_EMPTY_HELP(params);
    CMDP_CHECK_ARG_COUNT(params, 0, 0);
    CMDP_CHECK_OPT_CONFICT(__args.phrase && (__args.key || __args.iv), "--phrase", "--key/--iv");
    CMDP_CHECK_OPT_NEITHER(__args.phrase || __args.key, "--phrase", "--key/--iv");
    CMDP_CHECK_OPT_TOGETHER(XOR(__args.key, __args.iv), "--key", "--iv");
    CMDP_CHECK_OPT_CONFICT(__args.text && __args.infile, "--text", "--in");
    CMDP_CHECK_OPT_CONFICT(__args.outform && __args.hex, "--outform", "--hex");

    RESULT ret             = RET_FAIL;
    CC_SM4_PARAM sm4_param = {0};
    FORMAT_t inform        = __args.decrypt ? FORMAT_HEX : FORMAT_BIN;
    inform                 = cmd_get_format(__args.inform, inform);
    FORMAT_t outform       = __args.decrypt ? FORMAT_BIN : FORMAT_HEX;
    outform                = cmd_get_format(__args.outform, outform);
    if (__args.hex)
    {
        outform = FORMAT_HEX;
    }
    XIO *instream  = cmd_get_instream(__args.text, __args.infile, true);
    XIO *outstream = cmd_get_outstream(__args.outfile, true);
    instream       = cmd_wrap_stream(instream, inform);
    outstream      = cmd_wrap_stream(outstream, outform);

    if (__args.phrase)
    {
        pbkdf2_hmac_sm3_genkey(__args.phrase, strlen(__args.phrase), NULL, 0, 10000, sizeof(sm4_param),
                               (uint8_t *)&sm4_param);
    }
    else
    {
        hex_to_bytes(__args.key, strlen(__args.key), sm4_param.key, NULL);
        hex_to_bytes(__args.iv, strlen(__args.iv), sm4_param.iv, NULL);
    }

    LOG_SECRET_HEX("key=", sm4_param.key, sizeof(sm4_param.key));
    LOG_SECRET_HEX("iv =", sm4_param.iv, sizeof(sm4_param.iv));

    if (__args.decrypt)
    {
        ret = cc_sm4_cbc_decrypt(&sm4_param, instream, outstream);
    }
    else
    {
        ret = cc_sm4_cbc_encrypt(&sm4_param, instream, outstream);
    }

end:
    XIO_close(instream);
    XIO_close(outstream);
    clear_buffer(&sm4_param, sizeof(sm4_param));
    return ret == RET_OK ? CMDP_ACT_OK : CMDP_ACT_ERROR;
}
