#include "convert.h"


RESULT string_to_long(const char *str, long *result)
{
    if (str == NULL)
    {
        LOG_DBG("string_to_long: str is NULL");
        return RET_FAIL;
    }
    char *endptr = NULL;
    long len     = strtol(str, &endptr, 10);
    if (endptr == str || *endptr != '\0' || len <= 0)
    {
        LOG_ERR("invalid number: %s", str);
        return RET_FAIL;
    }
    *result = len;
    return RET_OK;
}
