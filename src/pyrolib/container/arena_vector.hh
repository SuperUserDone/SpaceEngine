#pragma once

#include "basic_vector.hh"
#include "pyrolib/log/assert.hh"
#include "pyrolib/memory/arena.hh"

namespace pyro {
namespace container {

template <typename T>
class arena_vector : public basic_vector<T> {
public:
  typedef T type;
  typedef arena_vector<T> this_type;

  void lt_init(size_t max_size = 4096) {
    arena.lt_init(max_size * sizeof(T));
    this->m_base = static_cast<T *>(arena.get_base());
    this->m_size = 0;
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

  template <typename U>
  void insert_back(U start, U end) {
    static_assert(
        std::is_same<typename std::remove_pointer<typename std::decay<U>::type>::type, T>::value,
        "Type needs to be the same!");
    arena.grow((this->m_size + (end - start)) * sizeof(T));

    for (U &i = start; i < end; i++) {
      this->m_base[this->m_size++] = *i;
    }
  }

  void reserve(size_t count) {
    arena.grow((this->m_size + count) * sizeof(T));
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