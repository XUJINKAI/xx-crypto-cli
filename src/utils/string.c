#include "string.h"

char *strstr_safe(const char *haystack, const char *needle)
{
    if (NULL == haystack || NULL == needle)
    {
        return NULL;
    }
    return strstr(haystack, needle);
}

bool is_ascii_string(const char *str, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (!is_ascii_char(str[i]))
        {
            return false;
        }
    }
    return true;
}

bool is_utf8_string(const char *str, size_t len)
{
    const char *utf8bom = "\xEF\xBB\xBF";
    if (len >= 3 && STREQ_CaseN(str, utf8bom, 3))
    {
        str += 3;
        len -= 3;
    }
    // TODO by github copilot
    for (size_t i = 0; i < len; i++)
    {
        if (is_ascii_char(str[i]))
        {
            continue;
        }
        if (i + 1 >= len)
        {
            return false;
        }
        if ((str[i] & 0xE0) == 0xC0)
        {
            if ((str[i + 1] & 0xC0) != 0x80)
            {
                return false;
            }
            i++;
        }
        else if ((str[i] & 0xF0) == 0xE0)
        {
            if (i + 2 >= len)
            {
                return false;
            }
            if ((str[i + 1] & 0xC0) != 0x80 || (str[i + 2] & 0xC0) != 0x80)
            {
                return false;
            }
            i += 2;
        }
        else if ((str[i] & 0xF8) == 0xF0)
        {
            if (i + 3 >= len)
            {
                return false;
            }
            if ((str[i + 1] & 0xC0) != 0x80 || (str[i + 2] & 0xC0) != 0x80 || (str[i + 3] & 0xC0) != 0x80)
            {
                return false;
            }
            i += 3;
        }
        else
        {
            return false;
        }
    }
    return true;
}
