#include "cc/hash.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "utils/converter.h"

#define IS_HASH_CALL(name) (STREQ_NoCase(name, "hash") || STREQ_NoCase(name, "h"))

static cmdp_action_t __process(cmdp_process_param_st *params);
static cmdp_flag_t __algo_flag(cmdp_flag_param_st *params)
{
    if (params->call_name != NULL && !IS_HASH_CALL(params->call_name))
    {
        return CMDP_FLAG_DISABLE;
    }
    return CMDP_FLAG_NONE;
}

static struct
{
    char *algo;
    char *intext;
    char *infile;
    char *informat;
    char *outformat;
    char *bufsize;
} args;

cmdp_command_st sc_hash = {
    .name = "hash",
    .desc = "sm3(default), md5, sha1, sha256...",
    .doc  = "Usage: xx hash [OPTIONS] <FILE>\n",
    .options =
        (cmdp_option_st[]){
            _opt_intext(args.intext, ),
            _opt_infile(args.infile, ),
            _opt_informat(args.informat, ),
            _opt_outformat(args.outformat, ),
            {0, "bufsize", "Update buffer size", CMDP_TYPE_STRING_PTR, &args.bufsize, "<size>"},
            {0, "algo", "Hash algorithm", CMDP_TYPE_STRING_PTR, &args.algo, "<ALGO>", .fn_flag = __algo_flag},
            CMDP_OPT_DOC("ALGO: sm3(default), md5, sha1, sha224, sha256, sha384, \n"
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
    CMDP_CHECK_ARG_COUNT(params, 0, 0);

    RESULT ret           = RET_FAIL;
    XIO *instream        = NULL;
    XIO *outstream       = NULL;
    const DIGEST *digest = NULL;

    FORMAT_t in_format  = cmd_get_format(args.informat, FORMAT_BIN);
    FORMAT_t out_format = cmd_get_format(args.outformat, FORMAT_HEX);
    LOG_VERBOSE("in_format=%s", format_to_string(in_format));
    LOG_VERBOSE("out_format=%s", format_to_string(out_format));

    instream  = cmd_get_instream(args.intext, args.infile, true);
    outstream = cmd_get_outstream(NULL, true);
    instream  = cmd_wrap_stream(instream, in_format);
    outstream = cmd_wrap_stream(outstream, out_format);

    if (args.algo == NULL)
    {
        args.algo = "sm3";
    }
    if (!IS_HASH_CALL(params->call_name))
    {
        args.algo = params->call_name;
    }
    digest = digest_from_name(args.algo);
    if (digest == NULL)
    {
        LOG_ERROR("Invalid algorithm: %s", args.algo);
        goto end;
    }
    LOG_VERBOSE("algo=%s", digest_name(digest));

    LOG_DBG_XIO("in : ", instream);
    LOG_DBG_XIO("out: ", outstream);

    if (instream == NULL || outstream == NULL)
    {
        goto end;
    }

    long bufsize = 0;
    if (RET_OK != string_to_long(args.bufsize, &bufsize, CONV_NULL_OK | CONV_SIZE))
    {
        LOG_ERROR("Invalid buffer size: %s", args.bufsize);
        goto end;
    }

    ret = cc_hash(digest, instream, outstream, bufsize);

end:
    XIO_CLOSE_SAFE(instream);
    XIO_CLOSE_SAFE(outstream);
    return RESULT_TO_CMDP_ACTION(ret);
}
