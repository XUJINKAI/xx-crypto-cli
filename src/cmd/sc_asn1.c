#include "cc/asn1.h"
#include "cc/format/hex.h"
#include "cc/format/pem.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "data/xbytes.h"
#include "global.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    char *intext;
    char *infile;
    char *outfile;
} args;

cmdp_command_st sc_asn1 = {
    .name = "asn1",
    .desc = "asn1 parser",
    .doc  = "Usage: asn1 [OPTIONS]\n",
    .options =
        (cmdp_option_st[]){
            _opt_intext(args.intext, ),
            _opt_infile(args.infile, ),
            _opt_outfile(args.outfile, ),
            {0},
        },
    .fn_process = __process,
};

static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_EMPTY_HELP(params);
    CMDP_CHECK_ARG_COUNT(params, 0, 0);

    RESULT ret      = RET_FAIL;
    XIO *instream   = NULL;
    XIO *outstream  = NULL;
    xbytes *allread = NULL;

    instream  = cmd_get_instream(args.intext, args.infile, true);
    outstream = cmd_get_outstream(args.outfile, true);
    allread   = XIO_read_all(instream, 0);

    uint8_t *p = xbytes_buffer(allread);
    size_t l   = xbytes_length(allread);

    bool is_string = is_utf8_string(p, l);
    LOG_DBG("is_string: %d", is_string);
    if (is_string)
    {
        xarray_of(CC_PEM_DATA) *pems = cc_pem_find_all((const char *)p);
        if (pems && xarray_count(pems) > 0)
        {
            LOG_DBG("pems count: %zu", xarray_count(pems));
            bool dump_label = xarray_count(pems) > 1;
            xarray_foreach(CC_PEM_DATA *, pem, pems)
            {
                if (dump_label)
                {
                    XIO_printf(outstream, "%.*s:\n", (int)pem->labellen, pem->label);
                }
                xbytes *bytes = cc_base64_decode((const uint8_t *)pem->data, pem->datalen, 0);
                if (!bytes)
                {
                    LOG_ERROR("base64 decode failed");
                }
                cc_asn1_print(outstream, xbytes_buffer(bytes), xbytes_length(bytes));
                xbytes_free_safe(bytes);
            }
        }
        xarray_free_safe(pems, NULL);

        xbytes *bytes = NULL; //cc_base64_decode(p, l);
        if (bytes)
        {
            LOG_DBG("base64 decode success");
        }
        if (!bytes)
        {
            bytes = cc_hex_decode(p, l, 0);
        }
        if (!bytes)
        {
            LOG_ERROR("unknown format");
            goto end;
        }
        cc_asn1_print(outstream, xbytes_buffer(bytes), xbytes_length(bytes));
        xbytes_free(bytes);
    }
    else
    {
        LOG_DBG("raw asn1 data");
        cc_asn1_print(outstream, p, l);
    }

    ret = RET_OK;

end:
    xbytes_free_safe(allread);
    xio_close_safe(instream);
    xio_close_safe(outstream);
    return RESULT_TO_CMDP_ACTION(ret);
}
