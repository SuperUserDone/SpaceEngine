#pragma once

#include "assetmanager/asset_set.hh"
#include "common/memory_arena.hh"
#include "common/memory_arena_typed.hh"
#include "data/app_state.hh"
#include "data/asset_storage.hh"
#include <atomic>

#define THREAD_SPLIT_COUNT 2

struct asset_data {
  asset_type type;
  
  union {
    texture_data texture;
    pipeline_data data;
  };
};

struct load_result {
  std::atomic_int semaphore;
  mem_arena_typed<asset_data> set;
  void *base;
  size_t count;
};

APIFUNC void asset_loader_upload_to_vram(app_state *state, load_result result);
APIFUNC load_result *asset_loader_load(mem_arena &arena, app_state *state, asset_set &set);
