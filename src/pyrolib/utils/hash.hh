#pragma once

#include "common/hash.hh"
#include <string.h>

namespace pyro {
namespace utils {

template <typename T>
struct hasher {

  static inline size_t hash(T in) {
    return (size_t)XXH3_64bits(&in, sizeof(in));
  }
};

template <>
inline size_t hasher<const char *>::hash(const char *string) {
  return HASH_KEY(string);
}

} // namespace utils
} // namespace pyro