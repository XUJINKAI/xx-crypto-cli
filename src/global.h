#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/xbytes.h"
#include "xio/xio.h"

#if !defined(NDEBUG) && !defined(_DEBUG)
#define _DEBUG
#endif

#define STACK_BUFFER_SIZE 4096

#define HAS_FLAG(a, b) (((a) & (b)) == (b))
#define XOR(a, b)      ((a) && !(b)) || (!(a) && (b))

typedef enum
{
    RET_OK   = 0,
    RET_FAIL = 1,
} RESULT;

typedef struct
{
    XIO *in;
    XIO *out;
    bool upper_hex;
#define G_HEX_FMT (g_state.upper_hex ? "%02X" : "%02x")
    bool no_color;
    bool no_eol; // true: add eol if out not ends with eol
    bool repl_mode;
    bool sigint_no_exit;
} APP_STATE;

extern APP_STATE g_state;

#include "gmssl/mem.h"
#define clear_buffer gmssl_secure_clear
#define free_safe(p)                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if (p)                                                                                                         \
        {                                                                                                              \
            free(p);                                                                                                   \
            p = NULL;                                                                                                  \
        }                                                                                                              \
    } while (0)

#include "utils/string.h"
#include "utils/unistd.h"

#include "log.h"

#endif /* __GLOBAL_H__ */
