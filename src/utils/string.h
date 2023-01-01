#pragma once

#include <stdbool.h>
#include <string.h>

#if defined(_MSC_VER)
#define strdup _strdup
#include <io.h>
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp  _stricmp
#endif

#define STREQ_Case(a, b)       (strcmp((a), (b)) == 0)
#define STREQ_NoCase(a, b)     (strcasecmp((a), (b)) == 0)
#define STREQ_CaseN(a, b, n)   (strncmp((a), (b), (n)) == 0)
#define STREQ_NoCaseN(a, b, n) (strncasecmp((a), (b), (n)) == 0)

#define is_space_char(c)    ((c) == ' ' || (c) == '\n' || (c) == '\t' || (c) == '\r')
#define is_line_end_char(c) ((c) == '\n' || (c) == '\r')
#define is_ascii_char(c)    ((c >= 0x20 && c <= 0x7e) || c == '\n' || c == '\t' || c == '\r')

char *strstr_safe(const char *haystack, const char *needle);
bool is_ascii_string(const char *str, size_t len);
bool is_utf8_string(const char *str, size_t len);
