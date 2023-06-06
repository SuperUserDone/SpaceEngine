#pragma once

#include "data/asset_storage.hh"
#include "data/asset_types.hh"
#include "pyrolib/container/hash_table.hh"
#include "pyrolib/memory/pool.hh"

enum asset_type {
  ASSET_TYPE_TEXTURE,
  ASSET_TYPE_PIPELINE,
  ASSET_TYPE_MESH,
  ASSET_TYPE_FRAMEBUFFER,
  ASSET_TYPE_FONT
};

struct asset_storage {
  pyro::container::hash_table<const char *, renderer_texture> texture_data;
  pyro::container::hash_table<const char *, renderer_pipeline> pipeline_data;
  pyro::container::hash_table<const char *, renderer_mesh> mesh_data;
  pyro::container::hash_table<const char *, renderer_framebuffer> framebuffer_data;
  pyro::container::hash_table<const char *, renderer_font> font_data;
};
