#pragma once

#include "common/hash_table.hh"
#include "common/memory_arena.hh"
#include "common/memory_arena_typed.hh"
#include "common/memory_pool.hh"
#include "data/asset_types.hh"

enum asset_type {
  ASSET_TYPE_TEXTURE,
  ASSET_TYPE_PIPELINE,
  ASSET_TYPE_MESH,
  ASSET_TYPE_FRAMEBUFFER
};

struct asset_index {
  asset_type type;
  void *value;
};

struct asset_storage {
  hash_table asset_lookup;

  mem_pool<renderer_texture> texture_data;
  mem_pool<renderer_pipeline> pipeline_data;
  mem_pool<renderer_mesh> mesh_data;
  mem_pool<renderer_framebuffer> framebuffer_data;
  mem_pool<asset_index> index_table;
};
