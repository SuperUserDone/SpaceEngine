#pragma once

#include "basic_vector.hh"
#include <stdlib.h>

namespace pyro {
namespace container {

template <typename T>
class heap_vector : basic_vector<T> {
  void lt_init(size_t prealloc_size = 32) {
    this->m_base = static_cast<T *>(malloc(prealloc_size * sizeof(T)));
    m_size_alloc = prealloc_size;
  }

  void lt_done() {
    free(this->m_base);
  }

  void reserve(size_t size) {
    if (m_size_alloc > size) {
      T *new_ptr = realloc(this->m_base, size);
    }
  }


public:
  size_t m_size_alloc;
};

} // namespace container
} // namespace pyro