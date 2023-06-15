#pragma once

#include <string.h>
#include <vcruntime_string.h>
#include <xxhash.h>

namespace pyro {
namespace container {

class string_id {
public:
  explicit inline string_id(const char *str, size_t len)
      : m_str(str), m_hash(XXH3_64bits(str, len)), m_len(len) {
  }
  explicit inline string_id(const char *str) : string_id(str, strlen(str)) {
  }

  string_id() {
  }

  size_t hash() const {
    return m_hash;
  }

  const char *string() const {
    return m_str;
  }

  friend inline bool operator==(const string_id &l, const string_id &r) {
    return l.m_hash == r.m_hash && (strcmp(l.m_str, r.m_str) == 0);
  }

  string_id lt_copy() const {
    char *new_str = new char[m_len + 1];
    memcpy(new_str, m_str, m_len);
    new_str[m_len] = 0;
    return string_id(new_str, m_len);
  }

  void lt_free() {
    delete[] m_str;
  }

private:
  const char *m_str;
  size_t m_hash;
  size_t m_len;
};

} // namespace container
} // namespace pyro

inline pyro::container::string_id operator"" _sid(const char *str, size_t len) {
  return pyro::container::string_id(str, len);
}
