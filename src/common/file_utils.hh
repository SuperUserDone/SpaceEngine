#pragma once

#include "common/memory_arena.hh"
#include <stdio.h>

static inline char *load_file(mem_arena &arena, const char *path) {
  FILE *fp;
  size_t len = 0;
  size_t read;

  fopen_s(&fp, path, "r");

  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *buf = PushArray(arena, char, len + 1);
  fread(buf, 1, len, fp);
  buf[len] = 0;

  fclose(fp);
  return buf;
}