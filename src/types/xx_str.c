#include "xx_str.h"
#include <stdlib.h>
#include <string.h>

XX_STR *XX_STR_new()
{
    XX_STR *str = calloc(1, sizeof(XX_STR));
    if (str == NULL)
    {
        return NULL;
    }
    str->ptr = NULL;
    str->len = 0;
    return str;
}
XX_STR *XX_STR_dup(char *__s)
{
    XX_STR *str = calloc(1, sizeof(XX_STR));
    if (str == NULL)
    {
        return NULL;
    }
    str->ptr = strdup(__s);
    if (!str->ptr)
    {
        free(str);
        return NULL;
    }
    str->len = strlen(__s);
    return str;
}
void XX_STR_free(XX_STR *str)
{
    if (str)
    {
        if (str->ptr)
        {
            free(str->ptr);
        }
        free(str);
    }
}
