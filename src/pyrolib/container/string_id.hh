#pragma once

#include <string.h>
#include <vcruntime.h>
#include <vcruntime_string.h>
#include <xxhash.h>

namespace pyro {
namespace container {

class string_id {
public:
  explicit inline string_id(const char *str, size_t len)
      : string_id(str, len, XXH3_64bits(str, len)) {
  }
  explicit inline string_id(const char *str) : string_id(str, strlen(str)) {
  }
  explicit inline string_id(const char *str, size_t len, size_t hash)
      : m_str(str), m_len(len), m_hash(hash) {
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
    return string_id(new_str, m_len, m_hash);
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
