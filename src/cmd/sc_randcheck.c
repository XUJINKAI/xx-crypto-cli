#include "cc/rand.h"
#include "cmd_helper.h"
#include "cmdparser.h"
#include "global.h"
#include "utils/convert.h"

static cmdp_action_t __process(cmdp_process_param_st *params);

static struct
{
    void *_;
} rand_args;

cmdp_command_st sc_randcheck = {
    .name = "randcheck",
    .desc = "Randomness test",
    .doc  = "Usage: randcheck [OPTIONS...] <FILE>\n"
           "\n",
    .options =
        (cmdp_option_st[]){
            {0},
        },
    .fn_process = __process,
};


static cmdp_action_t __process(cmdp_process_param_st *params)
{
    CMDP_CHECK_EMPTY_HELP(params);
    CMDP_CHECK_ARG_COUNT(params, 1, 1);
    char *testfile = CMDP_GET_ARG(params, 0);

    RESULT ret    = RET_FAIL;
    uint8_t *data = NULL;

    // read all file binary
    FILE *fp = fopen(testfile, "rb");
    if (fp == NULL)
    {
        LOG_ERROR("Failed to open file: %s", testfile);
        goto end;
    }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    data = malloc(len);
    if (data == NULL)
    {
        LOG_ERROR("Failed to alloc memory");
        goto end;
    }
    if (fread(data, 1, len, fp) != len)
    {
        LOG_ERROR("Failed to read file: %s", testfile);
        goto end;
    }
    fclose(fp);

    // memset(data, 0, len);
    ret = cc_random_check(data, len);
    printf("全部检测项: %s\n", ret == RET_OK ? "通过" : "失败");

end:
    if (data != NULL)
    {
        free(data);
    }
    return RESULT_TO_CMDP_ACTION(ret);
}
