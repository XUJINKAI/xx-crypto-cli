#pragma once

#include <stdlib.h>

typedef struct
{
    char *ptr;
    size_t len;
} XX_STR;

XX_STR *XX_STR_new();
XX_STR *XX_STR_dup(char *__s);
void XX_STR_free(XX_STR *str);
