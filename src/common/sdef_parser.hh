#pragma once

#include "common/memory_arena.hh"
#include "common/result.hh"
#include "win32_export.hh"
#include <stdint.h>

enum sdef_type {
  SDEF_TYPE_INTEGER,
  SDEF_TYPE_STRING,
  SDEF_TYPE_STRING_ARRAY,
};

struct sdef_property {
  char *name;
  sdef_type type;

  union {
    int64_t integer;
    char *string;
    struct {
      char *string_array;
      size_t array_count;
    };
  };
};

struct sdef_block {
  char *name;
  char *type;
  sdef_property *properties;
  size_t property_count;
};

struct sdef_dom {
  sdef_block *blocks;
  size_t block_count;
};

APIFUNC result<sdef_dom *> sdef_parse(mem_arena &arena,
                                      mem_arena &temp_arena,
                                      const char *str,
                                      size_t len);
