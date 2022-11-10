#include "cc/hash.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"

#define IS_HASH_CALL(name) (STREQ_NoCase(name, "hash") || STREQ_NoCase(name, "h"))

static cmdp_action_t __process(cmdp_process_param_st *params);
static cmdp_flag_t __algr_flag(cmdp_flag_param_st *params)
{
    if (params->call_name != NULL && !IS_HASH_CALL(params->call_name))
    {
        return CMDP_FLAG_DISABLE;
    }
    return CMDP_FLAG_NONE;
}

static struct
{
    char *algr;
    char *intext;
    char *informat;
    char *outformat;
    int bufsize;
} args;

cmdp_command_st sc_hash = {
    .name = "hash",
    .desc = "sm3(default), md5, sha1, sha256...",
    .doc  = "Usage: xx hash [OPTIONS] <FILE>\n",
    .options =
        (cmdp_option_st[]){
            _opt_intext(args.intext, ),
            _opt_informat(args.informat, ),
            _opt_outformat(args.outformat, ),
            {0, "bufsize", "Update buffer size", CMDP_TYPE_INT4, &args.bufsize},
            {0, "algr", "Hash algorithm", CMDP_TYPE_STRING_PTR, &args.algr, "<ALGR>", .fn_flag = __algr_flag},
            CMDP_OPT_DOC("ALGR: sm3(default), md5, sha1, sha224, sha256, sha384, \n"
                         "    sha512, sha512-224, sha512-256\n"
                         "Alias: 'xx sm3' is alias for 'xx hash --algr sm3', etc.\n"),
            {0},
        },
    .fn_process = __process,
    .alias_name = "h",
    .variant_cmds =
        (char *[]){"sm3", "md5", "sha1", "sha224", "sha256", "sha384", "sha512", "sha512-224", "sha512-256", NULL},
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    RESULT ret           = RET_FAIL;
    XIO *instream        = NULL;
    XIO *outstream       = NULL;
    const DIGEST *digest = NULL;

    FORMAT_t in_format  = cmd_get_format(args.informat, FORMAT_BIN);
    FORMAT_t out_format = cmd_get_format(args.outformat, FORMAT_HEX);

    instream  = cmd_get_instream(args.intext, CMDP_GET_ARG(params, 0), true);
    outstream = cmd_get_outstream(NULL, true);
    instream  = cmd_wrap_stream(instream, in_format);
    outstream = cmd_wrap_stream(outstream, out_format);

    if (args.algr == NULL)
    {
        args.algr = "sm3";
    }
    if (!IS_HASH_CALL(params->call_name))
    {
        args.algr = params->call_name;
    }
    digest = digest_from_name(args.algr);
    if (digest == NULL)
    {
        LOG_ERROR("Invalid algorithm: %s", args.algr);
        goto end;
    }

    LOG_DBG_XIO("in : ", instream);
    LOG_DBG_XIO("out: ", outstream);

    if (instream == NULL || outstream == NULL)
    {
        goto end;
    }

    ret = cc_hash_ex(digest, instream, outstream, args.bufsize);

end:
    XIO_CLOSE_SAFE(instream);
    XIO_CLOSE_SAFE(outstream);
    return RESULT_TO_CMDP_ACTION(ret);
}
