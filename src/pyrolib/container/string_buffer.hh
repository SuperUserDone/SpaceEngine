#pragma once

#include "pyrolib/container/array.hh"
#include "pyrolib/threading/spinlock.hh"

#include <string.h>

namespace pyro {
namespace container {

class string_buffer {
public:
  void lt_init(memory::heap &heap, size_t size = 65536) {
    m_data.lt_init(heap, size);
    m_end = 0;
    memset(&m_data[0], 0x0, size * sizeof(char));
  }

  void lt_done(memory::heap &heap) {
    m_data.lt_done(heap);
  }

  bool write(const char *in, size_t len) {
    m_lock.lock();
    size_t start = m_end.fetch_add(len);
    if (start + len >= m_data.size()) {
      // The buffer is full.
      return false;
    }

    // Copy the string;
    memcpy(&m_data[start], in, sizeof(char) * len);

    m_lock.unlock();
    return true;
  }

  bool write(const container::array<char> &str) {
    return write(&str[0], str.size() - 1);
  }

  bool write(const char *str) {
    return write(str, strlen(str));
  }

  const char *read_all(memory::arena &arena) {
    m_lock.lock();
    array<char> out;
    out.lt_copy(arena, m_data);

    memset(&m_data[0], 0x0, m_data.size() * sizeof(char));

    m_end = 0;

    m_lock.unlock();
    return &out[0];
  }

private:
  array<char> m_data;
  std::atomic_size_t m_end;
  threading::spinlock m_lock;
};

} // namespace container
} // namespace pyro