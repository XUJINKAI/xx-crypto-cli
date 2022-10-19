#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if !defined(NDEBUG) && !defined(_DEBUG)
#define _DEBUG
#endif

typedef enum
{
    RET_OK  = 0,
    RET_ERR = 1,
} RESULT;

#define X_SUCCESS 1
#define X_FAILURE 0

#include "gmssl/mem.h"
#define clear_buffer gmssl_secure_clear

#include "cross-platform/string.h"

#include "log.h"
#include "xio/xio.h"

#endif /* __GLOBAL_H__ */
