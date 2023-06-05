#pragma once

#include "pyrolib/memory/arena.hh"
#include <algorithm>

namespace pyro {
namespace memory {

template <typename T>
class pool {
public:
  void lt_init(size_t max_elemets) {
    m_obj_size = std::max(sizeof(T), sizeof(free_block));
    m_arena.lt_init(max_elemets * m_obj_size);
    m_first_free = nullptr;
  }

  void lt_done() {
    m_arena.lt_done();
  }

  T *alloc() {
    if (m_first_free) {
      free_block *addr = m_first_free;
      m_first_free = m_first_free->next;
      return reinterpret_cast<T *>(addr);
    }

    return static_cast<T *>(m_arena.push(m_obj_size));
  }

  void free(T *addr) {
    // When freeing add it to the list for the next allocation.
    free_block *old_free = m_first_free;
    reinterpret_cast<free_block*>(addr)->next = old_free;
    m_first_free = reinterpret_cast<free_block*>(addr);
  }

private:
  struct free_block {
    free_block *next;
  };

  pyro::memory::arena m_arena;
  free_block *m_first_free;
  size_t m_obj_size;
};

} // namespace memory
} // namespace pyro