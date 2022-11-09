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
    if (endptr == str || *endptr != '\0')
    {
        LOG_ERR("invalid number: %s", str);
        return RET_FAIL;
    }
    *result = len;
    return RET_OK;
}

RESULT string_to_long_ex(const char *__str, long *result)
{
    const char *str = __str;
    if (str == NULL)
    {
        LOG_DBG("string_to_long: str is NULL");
        return RET_FAIL;
    }

    int base = 10;
    if (strlen(str) > 2 && str[0] == '0' && str[1] == 'x')
    {
        base = 16;
        str += 2;
    }

    char *endptr = NULL;
    long len     = strtol(str, &endptr, base);
    if (endptr == str)
    {
        LOG_ERR("invalid number: %s", str);
        return RET_FAIL;
    }

    if (endptr[0] == 'k' || endptr[0] == 'K')
    {
        len *= 1024;
        endptr++;
    }
    else if (endptr[0] == 'm' || endptr[0] == 'M')
    {
        len *= 1024 * 1024;
        endptr++;
    }
    else if (endptr[0] == 'g' || endptr[0] == 'G')
    {
        len *= 1024 * 1024 * 1024;
        endptr++;
    }

    if (endptr[0] == 'b' || endptr[0] == 'B')
    {
        endptr++;
    }

    if (*endptr != '\0')
    {
        LOG_ERR("invalid number: %s", str);
        return RET_FAIL;
    }

    *result = len;
    return RET_OK;
}
