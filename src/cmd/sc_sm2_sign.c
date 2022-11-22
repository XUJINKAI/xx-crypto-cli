#include "cc/sm2.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "utils/converter.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *key;
    char *pass;
    char *id;
    char *instring;
    char *infile;
    char *bufsize;
} args;

cmdp_command_st sc_sm2_sign = {
    .name = "sign",
    .desc = "SM2 signature",
    .doc  = "Usage: sm2 sign [OPTIONS] --key [KEY]\n",
    .options =
        (cmdp_option_st[]){
            {0, "key", "Raw private key HEX or PEM file", CMDP_TYPE_STRING_PTR, &args.key, "<KEY>"},
            {0, "pass", "Password for the private key", CMDP_TYPE_STRING_PTR, &args.pass, "<PASS>"},
            {0, "id", "User ID", CMDP_TYPE_STRING_PTR, &args.id, "<ID>"},
            _opt_intext(args.instring, ),
            _opt_infile(args.infile, ),
            {0, "bufsize", "Update buffer size", CMDP_TYPE_STRING_PTR, &args.bufsize, "<size>"},
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

    instream  = cmd_get_instream(args.instring, args.infile, true);
    outstream = cmd_get_outstream(NULL, true);

    if (RET_OK != cc_sm2_try_load_private_key(&key, args.key, strlen(args.key), args.pass))
    {
        LOG_ERROR("Failed to load private key");
        goto end;
    }

    if (RET_OK != string_to_long(args.bufsize, &bufsize, CONV_SIZE | CONV_NULL_OK))
    {
        LOG_ERROR("Invalid buffer size: %s", args.bufsize);
        goto end;
    }

    ret = cc_sm2_sign(instream, &key, id, strlen(id), bufsize, sig, &siglen);
    XIO_write_base64(outstream, sig, siglen);

end:
    xio_close_safe(instream);
    xio_close_safe(outstream);
    clear_buffer(&key, sizeof(key));

    return RESULT_TO_CMDP_ACTION(ret);
}
