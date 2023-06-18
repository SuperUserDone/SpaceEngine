#pragma once

#include "pyrolib/utils/api.hh"
#include <stdio.h>

#define PYRO_LOG(fmt, level, ...) pyro::log::dolog("[" level "] " fmt "\n", __VA_ARGS__);

#define PYRO_LOG_(fmt, level, ...) PYRO_LOG(fmt, level, __VA_ARGS__)

#define PYRO_LOGI(fmt, ...) PYRO_LOG_(fmt, "I", __VA_ARGS__)
#define PYRO_LOGW(fmt, ...) PYRO_LOG_(fmt, "W", __VA_ARGS__)
#define PYRO_LOGE(fmt, ...) PYRO_LOG_(fmt, "E", __VA_ARGS__)

#define PYRO_LOGI_IF(expr, fmt, ...)                                                               \
  if ((expr)) {                                                                                    \
    PYRO_LOGI(fmt, __VA_ARGS__);                                                                   \
  }

#define PYRO_LOGW_IF(expr, fmt, ...)                                                               \
  if ((expr)) {                                                                                    \
    PYRO_LOGW(fmt, __VA_ARGS__);                                                                   \
  }

#define PYRO_LOGE_IF(expr, fmt, ...)                                                               \
  if ((expr)) {                                                                                    \
    PYRO_LOGE(fmt, __VA_ARGS__);                                                                   \
  }

namespace pyro {
namespace log {

PYROAPI void init();
PYROAPI void done();
PYROAPI void write(const char *str);
PYROAPI char *temp_alloc(size_t len);

template <typename... Args>
static inline void dolog(const char *format_string, Args... args) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
  size_t n = snprintf(nullptr, 0, format_string, args...);

  char *str = temp_alloc(n + 1);
  snprintf(str, n + 1, format_string, args...);

  write(str);

#pragma GCC diagnostic pop
}

} // namespace log
} // namespace pyro