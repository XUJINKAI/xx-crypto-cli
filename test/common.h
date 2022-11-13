#pragma once

#include "global.h"
#include "utest.h"
#include <stdio.h>

int xx_main(int argc, char *argv[], FILE *in_stream, FILE *out_stream, FILE *log_stream);

#define DIM(STRING)    "\x1b[2m" STRING "\x1b[0m"
#define countof(array) (sizeof(array) / sizeof(array[0]))

#define __COMBINE__(X, Y) X##Y
#define COMBINE(X, Y)     __COMBINE__(X, Y)

#define START_CMD()                                                                                                    \
    char *r_out      = NULL;                                                                                           \
    size_t _out_len  = 0;                                                                                              \
    FILE *out_stream = open_memstream(&r_out, &_out_len);                                                              \
    char *r_err      = NULL;                                                                                           \
    size_t _err_len  = 0;                                                                                              \
    FILE *err_stream = open_memstream(&r_err, &_err_len);                                                              \
    int r_code       = 0;

#define RUN_CMD(...)                                                                                                   \
    char *COMBINE(_arg_, __LINE__)[] = {__VA_ARGS__};                                                                  \
    r_code = xx_main(countof(COMBINE(_arg_, __LINE__)), COMBINE(_arg_, __LINE__), stdin, out_stream, err_stream);      \
    fflush(out_stream);                                                                                                \
    fflush(err_stream);

#define EXPECT_CMD(code, out, err)                                                                                     \
    EXPECT_EQ(code, r_code);                                                                                           \
    EXPECT_STREQ(out, r_out);                                                                                          \
    EXPECT_STREQ(err, r_err);

#define END_CMD()                                                                                                      \
    fclose(out_stream);                                                                                                \
    fclose(err_stream);                                                                                                \
    (void)r_code;                                                                                                      \
    free(r_out);                                                                                                       \
    r_out = NULL;                                                                                                      \
    free(r_err);                                                                                                       \
    r_err = NULL;
