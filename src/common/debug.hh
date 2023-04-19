#pragma once
#include "common/win32_export.hh"
#include "stdlib.h"

typedef void (*platform_error_func)(const char *msg);

APIFUNC extern platform_error_func platform_err;

#define SPACE_ASSERT(expr, fmt, ...)                                                               \
  [[unlikely]] if (!(expr)) {                                                                      \
    size_t needed = snprintf(NULL, 0, "Assert (" #expr ") failed! Message: " fmt, __VA_ARGS__);    \
    needed++;                                                                                      \
    char *buffer = (char *)malloc(needed);                                                         \
    sprintf(buffer, "Assert (" #expr ") failed! Message: " fmt, __VA_ARGS__);                      \
    platform_err(buffer);                                                                          \
  }