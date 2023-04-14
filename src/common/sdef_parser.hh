#pragma once

#include "common/memory_arena.hh"

#define MAX_NODE_CHILDREN 32

struct sdef_node {
  char *name;
  char *value;
  sdef_node **children;
  size_t child_count;
};

APIFUNC extern sdef_node *sdef_parse_string(mem_arena &arena, char *str, size_t len);