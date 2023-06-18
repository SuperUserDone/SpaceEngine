#pragma once

#include "log.hh"

#define PYRO_ASSERT(expr, fmt, ...)                                                                \
  if (!(expr)) {                                                                                   \
    PYRO_LOGE("Assert (" #expr ") failed! ABORT!" fmt, __VA_ARGS__);                               \
    abort();                                                                                       \
  }