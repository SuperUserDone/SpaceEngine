#pragma once

#include "assetmanager/asset_set.hh"
#include "common/memory_arena.hh"
#include "common/memory_arena_typed.hh"
#include "common/result.hh"
#include "data/app_state.hh"
#include "data/asset_storage.hh"
#include <atomic>

#define THREAD_SPLIT_COUNT 2

struct asset_data {
  asset_type type;
  const char *name;

  union {
    texture_data texture;
    pipeline_data pipeline;
  };
};

struct load_result {
  std::atomic_int semaphore;
  asset_data *set;
  size_t count;
};

APIFUNC result<> asset_loader_upload_to_vram(app_state *state, load_result *result);
APIFUNC result<load_result *> asset_loader_load_sync(mem_arena &arena,
                                                     app_state *state,
                                                     const asset_set &set);

APIFUNC result<> asset_loader_load_file_sync(app_state *state, const char *filename);
