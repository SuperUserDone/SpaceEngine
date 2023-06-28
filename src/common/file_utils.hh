#pragma once

// This is a temp piece of code to assist in development to quickly load a file into a string.
// Should be removed by release

#include "pyrolib/container/array.hh"
#include <pyrolib/memory/arena.hh>
#include <stdio.h>

static inline char *load_file(pyro::memory::arena &arena,
                              const char *path,
                              const char *mode = "r") {
  FILE *fp;
  size_t len = 0;
  size_t read;

  fopen_s(&fp, path, mode);
  if (!fp) {
    PYRO_LOGE("Could not open %s", path)
    return nullptr;
  }

  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  pyro::container::array<char> buf;
  buf.lt_init(arena, len + 1);
  read = fread(&buf[0], 1, len, fp);
  buf[read] = '\0';

  fclose(fp);
  return &buf[0];
}

static inline void *load_binary_file(pyro::memory::arena &arena,
                                     const char *path,
                                     size_t &len,
                                     const char *mode = "rb") {
  FILE *fp;
  size_t read;

  fopen_s(&fp, path, mode);
  if (!fp) {
    PYRO_LOGE("Could not open %s", path)
    return nullptr;
  }

  fseek(fp, 0, SEEK_END);
  len = ftell(fp);

  fseek(fp, 0, SEEK_SET);

  pyro::container::array<char> buf;
  buf.lt_init(arena, len);
  read = fread(&buf[0], 1, len, fp);

  len = read;

  fclose(fp);
  return &buf[0];
}