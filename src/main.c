#include "cmd/io_helper.h"
#include "cmdparser.h"
#include "cross-platform/unistd.h"
#include "global.h"
#include "gmssl/hex.h"
#include "gmssl/sm3.h"
#include "xcrypto.h"

static struct
{
    char *informat;
    char *infile;
    char *outformat;
    char *outfile;
} echo_args;

static struct
{
    char *fix_str;
    bool count_line;
    int interval_ms;
    int total_repeat;
    char *outformat;
    char *outfile;
} gen_args;

static struct
{
    bool decode;
    char *infile;
    char *outfile;
} hex_args;

static struct
{
    char *outformat;
    char *outfile;
} rand_args;

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


static cmdp_action_t test_process(cmdp_process_param_st *params);
static void echo_before(cmdp_before_param_st *params);
static cmdp_action_t echo_process(cmdp_process_param_st *params);
static void gen_before(cmdp_before_param_st *params);
static cmdp_action_t gen_process(cmdp_process_param_st *params);
static void hex_before(cmdp_before_param_st *params);
static cmdp_action_t hex_process(cmdp_process_param_st *params);
static void rand_before(cmdp_before_param_st *params);
static cmdp_action_t rand_process(cmdp_process_param_st *params);
static void sm4_before(cmdp_before_param_st *params);
static cmdp_action_t sm4_process(cmdp_process_param_st *params);

static cmdp_command_st main_cmdp = {
    .sub_commands =
        (cmdp_command_st[]){
            {
                .name       = "test",
                .alias_name = "t",
                .fn_process = test_process,
            },
            {
                .name = "echo",
                .desc = "echo the input",
                .doc  = "echo [OPTIONS] <TEXT>\n",
                .options =
                    (cmdp_option_st[]){
                        {'I', "in-format", "Input Format", CMDP_TYPE_STRING_PTR, &echo_args.informat, "<FORMAT>"},
                        {'O', "out-format", "Output Format", CMDP_TYPE_STRING_PTR, &echo_args.outformat, "<FORMAT>"},
                        {'i', "infile", "Input File", CMDP_TYPE_STRING_PTR, &echo_args.infile, "<filepath>"},
                        {'o', "outfile", "Output File", CMDP_TYPE_STRING_PTR, &echo_args.outfile, "<filepath>"},
                        {0},
                    },
                .fn_before  = echo_before,
                .fn_process = echo_process,
            },
            {
                .name = "gen",
                .desc = "Generate data",
                .options =
                    (cmdp_option_st[]){
                        {'s', "string", "Fixed String", CMDP_TYPE_STRING_PTR, &gen_args.fix_str, "<STRING>"},
                        {'l', "count-line", "Count Line", CMDP_TYPE_BOOL, &gen_args.count_line, NULL},
                        {'S', "sleep", "Sleep interval milliseconds", CMDP_TYPE_INT4, &gen_args.interval_ms,
                         "<MILLISECONDS>"},
                        {'r', "repeat", "Repeat", CMDP_TYPE_INT4, &gen_args.total_repeat, "<COUNT>"},
                        {'o', "outfile", "Output File", CMDP_TYPE_STRING_PTR, &gen_args.outfile, "<filepath>"},
                        {'O', "out-format", "Output Format", CMDP_TYPE_STRING_PTR, &gen_args.outformat, "<FORMAT>"},
                        {0},
                    },
                .fn_before  = gen_before,
                .fn_process = gen_process,
            },
            {
                .name = "hex",
                .desc = "hex encode/decode",
                .doc  = "hex [-d] STRING\n",
                .options =
                    (cmdp_option_st[]){
                        {'d', "decode", "decode", CMDP_TYPE_BOOL, &hex_args.decode},
                        {'r', "read", "input file", CMDP_TYPE_STRING_PTR, &hex_args.infile, "<file>"},
                        {'w', "write", "output file", CMDP_TYPE_STRING_PTR, &hex_args.outfile, "<file>"},
                        {0},
                    },
                .fn_before  = hex_before,
                .fn_process = hex_process,
            },
            {
                .name = "rand",
                .desc = "generate random bytes",
                .doc  = "rand [options] <len>\n",
                .options =
                    (cmdp_option_st[]){
                        {'o', "out", "write format", CMDP_TYPE_STRING_PTR, &rand_args.outformat, "<FORMAT>"},
                        {'w', "write", "output file", CMDP_TYPE_STRING_PTR, &rand_args.outfile, "<file>"},
                        {0},
                    },
                .fn_before  = rand_before,
                .fn_process = rand_process,
            },
            {
                .name = "sm4",
                .desc = "Encrypt or decrypt with SM4",
                .options =
                    (cmdp_option_st[]){
                        {'d', "decrypt", "decrypt", CMDP_TYPE_BOOL, &sm4_args.decrypt},
                        {'p', "phrase", "phrase to generate key and iv", CMDP_TYPE_STRING_PTR, &sm4_args.phrase},
                        {'k', "key", "key (16 bytes)", CMDP_TYPE_STRING_PTR, &sm4_args.key, "<hex>"},
                        {0, "iv", "IV (16 bytes)", CMDP_TYPE_STRING_PTR, &sm4_args.iv, "<hex>"},
                        {'I', "in-fmt", "Input Format", CMDP_TYPE_STRING_PTR, &sm4_args.informat, "<FORMAT>"},
                        {'O', "out-fmt", "Output Format", CMDP_TYPE_STRING_PTR, &sm4_args.outformat, "<FORMAT>"},
                        {'i', "infile", "Input File", CMDP_TYPE_STRING_PTR, &sm4_args.infile, "<filepath>"},
                        {'o', "outfile", "Output File", CMDP_TYPE_STRING_PTR, &sm4_args.outfile, "<filepath>"},
                        {0},
                    },
                .fn_before  = sm4_before,
                .fn_process = sm4_process,
            },
            {0},
        },
};

int main(int argc, char *argv[])
{
    return cmdp_run(argc - 1, argv + 1, &main_cmdp);
}


static cmdp_action_t test_process(cmdp_process_param_st *params)
{
    (void)params;
    XIO *in  = XIO_new_from_string("hello world", false);
    XIO *out = XIO_new_from_FILE(stdout, false);
    out      = XIO_new_filter_hex(out);
    XIO_printf(out, "123");
    XIO_close(in);
    XIO_close(out);
}

static void echo_before(cmdp_before_param_st *params)
{
    memset(&echo_args, 0, sizeof(echo_args));
}
static cmdp_action_t echo_process(cmdp_process_param_st *params)
{
    // LOG1("isatty(stdin) : %d", isatty(STDIN_FILENO));
    // LOG1("isatty(stdout): %d", isatty(STDOUT_FILENO));
    XIO_CMD_IN_PARAM in_param = {
        .filename     = echo_args.infile,
        .file_deffmt  = "bin",
        .argument     = params->argc > 0 ? params->argv[0] : NULL,
        .arg_deffmt   = "bin",
        .stdin_deffmt = "bin",
        .format       = echo_args.informat,
    };
    XIO *instream = XIO_new_cmd_instream(&in_param);

    XIO_CMD_OUT_PARAM out_param = {
        .filename      = echo_args.outfile,
        .file_deffmt   = "bin",
        .stdout_deffmt = "bin",
        .format        = echo_args.outformat,
    };
    XIO *outstream = XIO_new_cmd_outstream(&out_param);
    XIO_copy(instream, outstream);
    XIO_close(instream);
    XIO_close(outstream);
}


static void gen_before(cmdp_before_param_st *params)
{
    memset(&gen_args, 0, sizeof(gen_args));
    gen_args.fix_str     = "y";
    gen_args.interval_ms = 100;
    gen_args.count_line  = false;
}
static cmdp_action_t gen_process(cmdp_process_param_st *params)
{
    XIO_CMD_OUT_PARAM out_param = {
        .filename      = gen_args.outfile,
        .file_deffmt   = "bin",
        .stdout_deffmt = "bin",
        .format        = gen_args.outformat,
    };
    XIO *outstream = XIO_new_cmd_outstream(&out_param);
    for (int i = 0; i < gen_args.total_repeat || gen_args.total_repeat == 0; i++)
    {
        if (gen_args.count_line)
        {
            XIO_printf(outstream, "%d:%s\n", i, gen_args.fix_str);
        }
        else
        {
            XIO_printf(outstream, "%s\n", gen_args.fix_str);
        }
        XIO_flush(outstream);
        sleep_ms(gen_args.interval_ms);
    }
    XIO_close(outstream);
}


static void hex_before(cmdp_before_param_st *params)
{
    memset(&hex_args, 0, sizeof(hex_args));
}
static cmdp_action_t hex_process(cmdp_process_param_st *params)
{
    if (params->argc == 0 && params->opts == 0)
    {
        return CMDP_ACT_SHOW_HELP;
    }
    if (params->argc > 1)
    {
        LOG0("too many arguments");
        return CMDP_ACT_FAIL;
    }
    XIO *instream  = NULL;
    XIO *outstream = NULL;
    if (hex_args.infile)
    {
        instream = XIO_new_from_filename(hex_args.infile, "rb");
    }
    else
    {
        instream = XIO_new_from_string(params->argv[0], false);
    }
    if (hex_args.outfile)
    {
        outstream = XIO_new_from_filename(hex_args.outfile, "wb");
    }
    else
    {
        outstream = XIO_new_from_FILE(stdout, false);
    }
    if (hex_args.decode)
    {
        instream = XIO_new_filter_hex(instream);
    }
    else
    {
        outstream = XIO_new_filter_hex(outstream);
    }
    XIO_copy(instream, outstream);
    XIO_close(instream);
    XIO_close(outstream);
    return CMDP_ACT_OVER;
}


static void rand_before(cmdp_before_param_st *params)
{
    memset(&rand_args, 0, sizeof(rand_args));
}
static cmdp_action_t rand_process(cmdp_process_param_st *params)
{
    cmdp_action_t ret = CMDP_ACT_FAIL;
    if (params->argc == 0 && params->opts == 0)
    {
        return CMDP_ACT_SHOW_HELP;
    }
    if (params->argc > 1)
    {
        LOG0("too many arguments");
        return CMDP_ACT_FAIL;
    }
    char *endptr = NULL;
    long len     = strtol(params->argv[0], &endptr, 10);
    if (endptr == params->argv[0] || *endptr != '\0' || len <= 0)
    {
        LOG0("invalid length");
        return CMDP_ACT_FAIL;
    }
    XIO *out = NULL;
    if (rand_args.outfile != NULL)
    {
        out = XIO_new_from_filename(rand_args.outfile, "wb");
    }
    else
    {
        out = XIO_new_from_FILE(stdout, false);
    }
    if (rand_args.outformat == NULL && rand_args.outfile == NULL || rand_args.outformat == "hex")
    {
        out = XIO_new_filter_hex(out);
    }
    x_random_bytes(out, len);
    ret = CMDP_ACT_OVER;

end:
    XIO_close(out);
    return ret;
}


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
        LOG0("too many arguments");
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
        LOG0("Can't use argument text and input file together.");
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
