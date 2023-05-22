#pragma once

// This is the interface used to load assests from disk to memory and later VRAM. The loading of
// assets can be done async.

#include "assetmanager/asset_set.hh"
#include "common/memory_arena.hh"
#include "common/memory_arena_typed.hh"
#include "common/result.hh"
#include "data/app_state.hh"
#include "data/asset_storage.hh"
#include <atomic>

struct asset_data {
  asset_type type;
  const char *name;

  union {
    texture_data texture;
    pipeline_data pipeline;
  };
};

struct load_result {
  asset_data *set;
  size_t count;
};

struct async_load_result {
  size_t count;
  std::atomic_size_t loaded;
  std::atomic_size_t i;
  std::string error;
  std::atomic_bool has_error;
  load_result result;
};

struct async_load_result_info {
  size_t count;
  size_t loaded;
};

// Load assets from memory into vram. This runs synchonously, but does extra error checking as
// the asset loading could have failed. This blocks if the assets are not yet available.
APIFUNC result<> asset_loader_upload_to_vram(app_state *state, async_load_result *result);

// Load the assets asynchornously. The arena should not be used elsewhere while loading, and the set
// must remain available until after uploaded to vram.
APIFUNC async_load_result *asset_loader_load_async(mem_arena &arena,
                                                   app_state *state,
                                                   const asset_set &set);

// Get loading progress from the async load result.
APIFUNC result<async_load_result_info> asset_loader_async_query(async_load_result &result);

// Helper funcition Load file and block until complete
APIFUNC result<> asset_loader_load_file_sync(app_state *state, const char *filename);

// Helper function for an async load. Does not upload to VRAM, needs to be done maunually. Arena
// should not be used before all assets are finished loading.
APIFUNC result<async_load_result *> asset_loader_load_file_async(mem_arena &arena,
                                                                 app_state *state,
                                                                 const char *filename);
