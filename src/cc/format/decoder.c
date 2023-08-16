#include "decoder.h"

char *cc_str_while_cb(char *str, bool (*cb)(char c))
{
    while (*str && cb(*str))
    {
        str++;
    }
    return str;
}
