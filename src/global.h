#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
    FILE *in;
    FILE *out;
    bool uppercase;
#define G_HEX_FMT (g_state.uppercase ? "%02X" : "%02x")
    bool no_color;
    bool repl_mode;
    bool sigint_no_exit;
} APP_STATE;

extern APP_STATE g_state;

#include "gmssl/mem.h"
#define clear_buffer gmssl_secure_clear

#include "utils/string.h"
#include "utils/unistd.h"

#include "types/xx_mem.h"
#include "types/xx_str.h"

#include "utils/log.h"
#include "xio/xio.h"

#endif /* __GLOBAL_H__ */
