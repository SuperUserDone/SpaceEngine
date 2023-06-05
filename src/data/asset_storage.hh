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

struct asset_index {
  asset_type type;
  void *value;
};

struct asset_storage {
  pyro::container::hash_table<const char *, asset_index> asset_lookup;

  pyro::memory::pool<renderer_texture> texture_data;
  pyro::memory::pool<renderer_pipeline> pipeline_data;
  pyro::memory::pool<renderer_mesh> mesh_data;
  pyro::memory::pool<renderer_framebuffer> framebuffer_data;
  pyro::memory::pool<renderer_font> font_data;
};
