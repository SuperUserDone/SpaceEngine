#pragma once

#include "common/memory_arena.hh"
#include "win32_export.hh"
#include <stdint.h>

enum sdef_type {
  SDEF_TYPE_INTEGER,
  SDEF_TYPE_STRING,
  SDEF_TYPE_STRING_ARRAY,
  SDEF_TYPE_ROOT,
  SDEF_TYPE_CORE,
};

struct ast_node {
  sdef_type type;

  const char *name;

  union {
    int64_t integer;
    const char *string;
    const char **string_array;

    struct {
      ast_node *children;
      size_t child_count;
    } collection;
  };
};

APIFUNC ast_node *sdef_parse(mem_arena &arena, mem_arena &temp_arena, const char *str, size_t len);
