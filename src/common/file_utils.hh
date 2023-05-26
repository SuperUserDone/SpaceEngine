#pragma once

// This is a temp piece of code to assist in development to quickly load a file into a string.
// Should be removed by release

#include "memory/memory_arena.hh"
#include <stdio.h>

static inline char *load_file(mem_arena &arena, const char *path) {
  FILE *fp;
  size_t len = 0;
  size_t read;

  fopen_s(&fp, path, "r");

  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *buf = (char *)arena_push_atomic(arena, sizeof(char) * (len + 1));
  read = fread(buf, 1, len, fp);
  buf[read] = '\0';

  fclose(fp);
  return buf;
}
