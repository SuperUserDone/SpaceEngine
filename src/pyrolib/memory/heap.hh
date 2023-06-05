#pragma once

#include <stdlib.h>

namespace pyro {
namespace memory {

class heap {
public:
  void *alloc(size_t size) {
    return calloc(size, 1);
  }

  void free(void *allocation) {
    ::free(allocation);
  }

  template <typename T>
  T *alloc() {
    return reinterpret_cast<T *>(alloc(sizeof(T)));
  }

  void *realloc(void *allocation, size_t size) {
    return ::realloc(allocation, size);
  }
};

} // namespace memory
} // namespace pyro