#include "cc/format/hex.h"
#include "cc/sm2.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *sk;
    bool unzip;
    bool zip;
    bool pem;
    char *outfile;
} args;

cmdp_command_st sc_sm2_keygen = {
    .name = "keygen",
    .desc = "SM2 Key Generator",
    .doc  = "Usage: sm2 keygen [OPTIONS]\n",
    .options =
        (cmdp_option_st[]){
            {0, "sk", "Specify the private key", CMDP_TYPE_STRING_PTR, &args.sk, "<HEX>"},
            {0, "unzip", "Output uncompressed public key", CMDP_TYPE_BOOL, &args.unzip},
            {0, "zip", "Output compressed public key", CMDP_TYPE_BOOL, &args.zip},
            {0, "pem", "Output the key in PEM format", CMDP_TYPE_BOOL, &args.pem},
            _opt_outfile(args.outfile, ),
            {0},
        },
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_ARG_COUNT(params, 0, 0);

    XIO *outstream = NULL;
    outstream      = cmd_get_outstream(args.outfile, true);

    SM2_KEY key;
    if (args.sk)
    {
        uint8_t sk[32];
        if (RET_OK != cc_hex_dec_expect_len(args.sk, 32, sk))
        {
            LOG_ERROR("Invalid sk (private key).");
            goto end;
        }
        sm2_key_set_private_key(&key, sk);
        clear_buffer(sk, 32);
    }
    else
    {
        sm2_key_generate(&key);
    }

    if (args.unzip + args.zip + args.pem == 0)
    {
        args.unzip = true;
    }

    if (args.unzip)
    {
        cc_sm2_private_key_print_raw(outstream, &key);
        cc_sm2_public_key_print_raw(outstream, &key);
    }
    if (args.zip)
    {
        cc_sm2_private_key_print_raw(outstream, &key);
        cc_sm2_public_key_print_compressed_raw(outstream, &key);
    }
    if (args.pem)
    {
        cc_sm2_private_key_print_pem(outstream, &key);
        cc_sm2_public_key_print_pem(outstream, &key);
    }

end:
    clear_buffer(&key, sizeof(key));
    return CMDP_ACT_OK;
}
