#pragma once

#include <stdint.h>

#if defined(__linux__)
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <windows.h>

#include <io.h>
#define isatty _isatty

#include <process.h>
#define getpid _getpid
#endif

#define FILE_EXISTS(FILE)   (access((FILE), F_OK) == 0)
#define FILE_CAN_READ(FILE) (access((FILE), R_OK) == 0)
/* 若文件不存在，则返回false
想要判断地址是否可写入，需要使用FILE_PATH_CAN_WRITE宏 */
#define FILE_CAN_WRITE(FILE) (access((FILE), W_OK) == 0)
#define FILE_CAN_EXEC(FILE)  (access((FILE), X_OK) == 0)
/* 文件不存在或者存在且可写
指示可以向地址写入文件 */
#define FILE_PATH_CAN_WRITE(FILE) (!FILE_EXISTS(FILE) || FILE_CAN_WRITE(FILE))

void sleep_ms(int milliseconds);
int64_t time_ns(void);

#if defined(__unix__)
#define __packed           __attribute__((packed))
#define __aligned(n)       __attribute__((aligned(n)))
#define __printflike(a, b) __attribute__((format(printf, a, b)))
#define __weak             __attribute__((weak))
#define __must_check       __attribute__((warn_unused_result))
#define __unused           __attribute__((unused))
#define __used             __attribute__((used))
#define __nullend          __attribute__((sentinel))
#define __deprecated       __attribute__((deprecated))
#else
#define __packed
#define __aligned(n)
#define __printflike(a, b)
#define __weak
#define __must_check
#define __unused
#define __used
#define __nullend
#define __deprecated
#endif
