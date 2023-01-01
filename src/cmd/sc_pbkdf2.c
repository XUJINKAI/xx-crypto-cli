#include "cc/format/hex.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "gmssl/pbkdf2.h"
#include "utils/converter.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *digest;
    char *pass;
    char *salt;
    char *iter;
    char *outlen;
} args;

cmdp_command_st sc_pbkdf2 = {
    .name = "pbkdf2",
    .desc = "PBKDF2 key derivation function",
    .doc  = "Usage: pbkdf2 [OPTIONS] -p <PASS> -n <OUTLEN>\n",
    .options =
        (cmdp_option_st[]){
            {'n', NULL, "Output Length", CMDP_TYPE_STRING_PTR, &args.outlen, "<N>"},
            {'p', "pass", "Password", CMDP_TYPE_STRING_PTR, &args.pass, "<PASS>"},
            {'s', "salt", "Salt", CMDP_TYPE_STRING_PTR, &args.salt, "<HEX>"},
            {0, "iter", "Iteration, default: 10000", CMDP_TYPE_STRING_PTR, &args.iter, "<N>"},
            {0, "digest", "Digest Algorithm, default: sm3", CMDP_TYPE_STRING_PTR, &args.digest, "<DIGEST>"},
            {0},
        },
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_EMPTY_HELP(params);
    CMDP_CHECK_ARG_COUNT(params, 0, 0);

    char *pass      = args.pass;
    size_t passlen  = strlen(pass);
    xbytes *salt    = NULL;
    long iter       = 10000;
    long outlen     = 0;
    uint8_t *outbuf = NULL;
    RESULT ret      = RET_FAIL;
    XIO *outstream  = NULL;

    if (RET_OK != string_to_long(args.iter, &iter, CONV_SIZE | CONV_NULL_OK))
    {
        LOG_ERROR("Invalid iteration");
        goto end;
    }
    if (RET_OK != string_to_long(args.outlen, &outlen, CONV_SIZE))
    {
        LOG_ERROR("Invalid outlen");
        goto end;
    }

    const DIGEST *digest = args.digest ? digest_from_name(args.digest) : DIGEST_sm3();
    if (digest == NULL)
    {
        LOG_ERROR("Invalid digest");
        goto end;
    }

    salt      = args.salt ? cc_hex_decode(args.salt, strlen(args.salt), 0) : xbytes_new(0);
    outbuf    = (uint8_t *)malloc(outlen);
    outstream = cmd_get_outstream(NULL, true);

    if (1 != pbkdf2_genkey(digest, pass, passlen, xbytes_buffer(salt), xbytes_length(salt), iter, outlen, outbuf))
    {
        LOG_ERROR("pbkdf2_genkey failed");
        goto end;
    }
    XIO_write_hex(outstream, outbuf, outlen);

#if 0 // same
    XIO_printf(outstream, "\n");
    if (1 != pbkdf2_hmac_sm3_genkey(pass, passlen, salt->ptr, salt->len, iter, outlen, outbuf))
    {
        LOG_ERROR("pbkdf2_hmac_sm3_genkey failed");
        goto end;
    }
    XIO_write_hex(outstream, outbuf, outlen);
#endif

    ret = RET_OK;

end:
    free_safe(outbuf);
    xbytes_free_safe(salt);
    xio_close_safe(outstream);

    return RESULT_TO_CMDP_ACTION(ret);
}
