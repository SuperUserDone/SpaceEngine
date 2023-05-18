#pragma once

// This is the parser interface for the sdef file format.
// Using the interface you can parse a string into a simple dom.
//
// Each file consistis of a number of blocks each containing a name, a type and a group of
// properties
//
// Properties can be a string, integer or array. Arrays are declared in square brackets with strings
// as elements. Elements can but need not be seperated by a comma
//
// Example file
// [type] name
// prop1 "prop_value"
// prop2 ["prop_value1", "prop_value2"]
// prop3 ["prop_value3" "prop_value4"]
//
// Note that in the dom arrays are stored as null terminated strings concatenated together

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
      size_t total_len;
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
                                      const char *str,
                                      size_t len);
