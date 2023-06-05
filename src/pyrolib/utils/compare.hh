#pragma once

#include <string.h>

namespace pyro {
namespace utils {

template <typename T>
struct comparison {
  static inline bool compare(T a, T b) {
    return a == b;
  }
};

template <>
inline bool comparison<const char *>::compare(const char *string1, const char *string2) {
  return strcmp(string1, string2) == 0;
}

template <typename T>
struct comparison<T*> {
};

} // namespace utils
} // namespace pyrolib