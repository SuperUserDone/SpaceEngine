#pragma once

#include "common/memory_arena.hh"
#include "common/memory_arena_typed.hh"
#include "common/memory_pool.hh"
#include "data/asset_types.hh"

struct asset_bundle {
  const char *name;
  mem_pool<renderer_texture> texture_data;
  mem_pool<renderer_pipeline> pipeline_data;
  mem_pool<renderer_mesh> mesh_data;
};

struct asset_storage {
  mem_pool<asset_bundle> bundles;
};
