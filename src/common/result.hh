#pragma once

// A simple result primitive inspired by rust's result primitive we use std::string here as I want
// to experiment in bringing some limited features of c++ in, and as we dont care about allocation
// behavoir in error handlers it should be fine

#include "common/debug.hh"
#include <stdio.h>
#include <string.h>
#include <string>

#define result_forward_err(name, expr)                                                             \
  auto _res_##name = (expr);                                                                       \
  if (!_res_##name.ok())                                                                           \
    return _res_##name;                                                                            \
  auto name = _res_##name.get();

enum result_type {
  RESULT_OK,
  RESULT_ERR
};

template <typename T = bool>
struct result {
  result_type type;
  std::string error;

  T value;

  operator T() {
    return get();
  }

  T get() {
    SPACE_ASSERT(type == RESULT_OK,
                 "Attempt to read value from error result. Message: %s",
                 error.c_str());
    return value;
  };

  T get_no_err() {
    SPACE_ASSERT_NODIE(type == RESULT_OK,
                       "Attempt to read value from error result. Message: %s",
                       error.c_str());
    return value;
  }

  // Only used to copy using the arena_forward_err macro above
  template <typename K>
  result(result<K> &res) {
    this->type = res.type;
    SPACE_ASSERT(!res.ok(), "Copying an error from an result with no error");
    this->error = res.error;
  }

  result() {
  }

  bool ok() {
    return type == RESULT_OK;
  }
};

template <typename T = bool>
static inline result<T> result_ok(T val) {
  result<T> r;

  r.value = val;
  r.type = RESULT_OK;

  return r;
}

template <typename T = bool>
static inline result<T> result_err(const char *err) {
  result<T> r;

  int len = strlen(err);

  r.error = err;
  r.type = RESULT_ERR;

  return r;
}

template <typename T = bool, typename... args>
static inline result<T> result_err(const char *err, args... a) {
  result<T> r;

  // TODO better way to do this?
  int sz = snprintf(nullptr, 0, err, a...);
  char *err_buf = (new char[sz + 1]);
  sprintf_s(err_buf, sz + 1, err, a...);

  r.error = err_buf;
  r.type = RESULT_ERR;

  delete[] err_buf;

  return r;
}
