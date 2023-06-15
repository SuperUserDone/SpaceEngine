#pragma once

#include "common/hash.hh"
#include "pyrolib/container/string_id.hh"
#include <string.h>
#include <type_traits>
#include <utility>

namespace pyro {
namespace utils {

template <typename T>
struct hasher {

  static inline size_t hash(const T &in) {
    static_assert(!std::is_same_v<std::decay_t<T>,const char *>,
                  "Do not use const char *for key type, use string_id instead");
    return (size_t)XXH3_64bits(&in, sizeof(T));
  }
};

template <>
inline size_t hasher<container::string_id>::hash(const container::string_id &sid) {
  return sid.hash();
}

} // namespace utils
} // namespace pyro