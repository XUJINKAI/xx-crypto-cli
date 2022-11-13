#include "converter.h"
#include "global.h"

RESULT string_to_long(const char *__str, long *__result, int flag)
{
    const char *str = __str;
    if (str == NULL)
    {
        LOG_DBG("string_to_long: str is NULL");
        return HAS_FLAG(flag, CONV_NULL_OK) ? RET_OK : RET_FAIL;
    }

    int base = 10;
    if (HAS_FLAG(flag, CONV_HEX_PREFIX) && str[0] == '0' && str[1] == 'x')
    {
        base = 16;
        str += 2;
    }

    char *endptr = NULL;
    long result  = strtol(str, &endptr, base);
    if (endptr == str)
    {
        return RET_FAIL;
    }
    if (HAS_FLAG(flag, CONV_NEGATIVE_FAIL) && result < 0)
    {
        return RET_FAIL;
    }

    if (HAS_FLAG(flag, CONV_KMGB_SUFFIX))
    {
        if (endptr[0] == 'k' || endptr[0] == 'K')
        {
            result *= 1024;
            endptr++;
        }
        else if (endptr[0] == 'm' || endptr[0] == 'M')
        {
            result *= 1024 * 1024;
            endptr++;
        }
        else if (endptr[0] == 'g' || endptr[0] == 'G')
        {
            result *= 1024 * 1024 * 1024;
            endptr++;
        }

        if (endptr[0] == 'b' || endptr[0] == 'B')
        {
            endptr++;
        }
    }

    if (*endptr != '\0')
    {
        return RET_FAIL;
    }

    *__result = result;
    return RET_OK;
}
