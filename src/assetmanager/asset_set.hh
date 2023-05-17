#pragma once

#include "common/memory_arena.hh"
#include "common/win32_export.hh"
#include "data/asset_storage.hh"
#include "data/asset_types.hh"
#include "common/result.hh"

struct asset_descriptor {
  asset_type type;
  const char *name;
  union {
    struct {
      texture_filter upsample;
      texture_filter downsample;
      const char *file_path;
    } texture;
    struct {
      const char *vertex;
      const char *fragment;
    } pipeline;
  };
};

struct asset_set {
  asset_descriptor *descriptors;
  size_t count;
};

APIFUNC result<asset_set> asset_set_load_from_file(mem_arena &arena, const char *filename);
