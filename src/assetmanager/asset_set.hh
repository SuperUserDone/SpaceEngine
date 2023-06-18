#pragma once

#include "common/result.hh"
#include "common/win32_export.hh"
#include "data/asset_storage.hh"
#include "pyrolib/container/array.hh"
#include <pyrolib/memory/arena.hh>

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
      char **uniforms;
      size_t uniforms_count;
    } pipeline;
    struct {
      const char *path;
    } font;
  };
};

struct asset_set {
  pyro::container::array<asset_descriptor> descriptors;
};

APIFUNC result<asset_set> asset_set_load_from_file(pyro::memory::arena &arena,
                                                   const char *filename);
