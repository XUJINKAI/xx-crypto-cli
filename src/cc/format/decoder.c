#include "decoder.h"

char *cc_str_while_cb(char *str, bool (*cb)(char c))
{
    while (*str && cb(*str))
    {
        str++;
    }
    return str;
}

/* 
pem
 */

static const char *__skip_to_pem_label_end(const char *text)
{
    const char *p = text;
    while (*p && *p != '-' && *p != '\n' && *p != '\r')
    {
        p++;
    }
    return *p == '-' ? p : NULL;
}
static const char *__skip_pem_dash_tail(const char *text)
{
    if (text[0] == '-' && text[1] == '-' && text[2] == '-' && text[3] == '-' && text[4] == '-')
    {
        const char *p = text + 5;
        while (*p == '\n' && *p == '\r')
        {
            p++;
        }
        return p;
    }
    return NULL;
}
/* 
p1         p2   p3   p4
-----BEGIN LABEL-----
base64
-----END LABEL-----
p5       p6   p7   p8
 */
static const char *__find_pem_1(const char *text, CC_PEM_DATA *out)
{
    const char *pem_begin = "-----BEGIN ";
    const char *pem_end   = "-----END ";
    const char *p1 = NULL, *p2 = NULL, *p3 = NULL, *p4 = NULL, *p5 = NULL, *p6 = NULL, *p7 = NULL, *p8 = NULL;

    p1 = strstr(text, pem_begin);
    if (!p1)
    {
        return NULL;
    }
    p2 = p1 + strlen(pem_begin);
    p3 = __skip_to_pem_label_end(p2);
    if (!p3)
    {
        return p2;
    }
    p4 = __skip_pem_dash_tail(p3);
    if (!p4)
    {
        return p3;
    }

    p5 = strstr(p4, pem_end);
    if (!p5)
    {
        return p4;
    }
    p6 = p5 + strlen(pem_end);
    p7 = __skip_to_pem_label_end(p6);
    if (!p7)
    {
        return p6;
    }
    p8 = __skip_pem_dash_tail(p7);
    if (!p8)
    {
        return p7;
    }

    if (p3 - p2 != p7 - p6)
    {
        return p8;
    }

    return p8;
}

RESULT cc_pem_find(const char *text, CC_PEM_DATA *out)
{
    if (out)
    {
        memset(out, 0, sizeof(CC_PEM_DATA));
    }
    const char *p = text;
    while (p)
    {
        p = __find_pem_1(p, out);
    }
    return p ? RET_OK : RET_FAIL;
}

xarray_of(CC_PEM_DATA) * cc_pem_find_all(const char *text)
{
    if (!text)
    {
        return NULL;
    }
    xarray_of(CC_PEM_DATA) *result = xarray_new_of(CC_PEM_DATA, 5);

    const char *p   = text;
    CC_PEM_DATA pem = {0};
    while (p)
    {
        if (RET_OK == cc_pem_find(p, &pem))
        {
            p = pem.pem_end;
            xarray_add(result, &pem);
        }
        else
        {
            p = NULL;
        }
    }

    return result;
}
