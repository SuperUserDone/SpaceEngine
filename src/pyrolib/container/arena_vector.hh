#pragma once

#include "basic_vector.hh"
#include "pyrolib/log/assert.hh"
#include "pyrolib/memory/arena.hh"

namespace pyro {
namespace container {

template <typename T>
class arena_vector : public basic_vector<T> {
public:
  void lt_init(size_t max_size = 4096) {
    arena.lt_init(max_size * sizeof(T));
    this->m_base = static_cast<T *>(arena.get_base());
  }

  void lt_done() {
    arena.lt_done();
  }

  typedef T *iterator;

  void push_back(const T &t) {
    size_t size = this->m_size++;

    arena.grow(this->m_size * sizeof(T));
    this->m_base[size] = t;
  }

  void pop_back() {
    PYRO_ASSERT(this->m_size > 0, "Cannot free element when no elements exist!");
    this->m_size--;
  }

private:
  pyro::memory::arena arena;
};

} // namespace container
} // namespace pyro