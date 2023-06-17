#pragma once

#include "pyrolib/container/array.hh"
#include "pyrolib/container/string_buffer.hh"
#include "pyrolib/memory/scratch_arena.hh"

#include <stdio.h>

#define PYRO_LOG(fmt, file, line, level, ...)                                                      \
  pyro::log::dolog("[" level "] " fmt "\n", __VA_ARGS__);

#define PYRO_LOG_(fmt, level, ...) PYRO_LOG(fmt, file, line, level, __VA_ARGS__)

#define PYRO_LOGI(fmt, ...) PYRO_LOG_(fmt, "I", __VA_ARGS__)
#define PYRO_LOGW(fmt, ...) PYRO_LOG_(fmt, "W", __VA_ARGS__)
#define PYRO_LOGE(fmt, ...) PYRO_LOG_(fmt, "E", __VA_ARGS__)

namespace pyro {
namespace log {

PYROAPI void init();
PYROAPI void done();
PYROAPI void write(const container::array<char> &str);

template <typename... Args>
static inline void dolog(const char *format_string, Args... args) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
  size_t n = snprintf(nullptr, 0, format_string, args...);
  memory::scratch_arena a = memory::scratch_get();

  container::array<char> str;
  str.lt_init(a, n + 1);
  snprintf(&str[0], str.size(), format_string, args...);

  write(str);

  memory::scratch_free(a);
#pragma GCC diagnostic pop
}

} // namespace log
} // namespace pyro