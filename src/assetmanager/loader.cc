#include "loader.hh"
#include "assetmanager/asset_set.hh"
#include "assetmanager/assetmanager.hh"
#include "assetmanager/loader.hh"
#include "common/file_utils.hh"
#include "common/hash.hh"
#include "common/result.hh"
#include "data/asset_storage.hh"
#include "memory/memory_scratch_arena.hh"
#include "tracy/Tracy.hpp"
#include <pyrolib/memory/arena.hh>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

result<asset_data> loader_load_texture(pyro::memory::arena &arena, const asset_descriptor &desc) {
  ZoneScopedN("Texture Load");

  int x, y, c;
  void *data = stbi_load(desc.texture.file_path, &x, &y, &c, 0);

  if (!data)
    return result_err<asset_data>("Could not load file %s from disk", desc.texture.file_path);

  void *new_data = arena.push(x * y * c);
  memcpy(new_data, data, x * y * c);

  stbi_image_free(data);

  asset_data a;
  a.type = ASSET_TYPE_TEXTURE;
  a.texture.data = new_data;
  a.texture.h = y;
  a.texture.w = x;
  a.texture.edge = TEX_EDGE_REPEAT;
  a.texture.upscale = desc.texture.upsample;
  a.texture.downscale = desc.texture.downsample;
  switch (c) {
  case 1:
    a.texture.format = TEX_FORMAT_R;
    break;
  case 2:
    a.texture.format = TEX_FORMAT_RG;
    break;
  case 3:
    a.texture.format = TEX_FORMAT_SRGB;
    break;
  case 4:
    a.texture.format = TEX_FORMAT_SRGBA;
    break;
  default:
    return result_err<asset_data>("Texture file has an unsupported number of channels %d", c);
  }

  return result_ok(a);
}

result<asset_data> loader_load_pipeline(pyro::memory::arena &arena, const asset_descriptor &desc) {
  ZoneScopedN("Pipeline Load");
  asset_data a;
  a.type = ASSET_TYPE_PIPELINE;
  a.pipeline.uniform_count = desc.pipeline.uniforms_count;
  // Reason why set needs to remain in scope until asset are uploaded to GPU. I might change this
  // later.
  a.pipeline.uniform_names = (const char **)desc.pipeline.uniforms;
  a.pipeline.fragment_shader = load_file(arena, desc.pipeline.fragment);
  a.pipeline.vertex_shader = load_file(arena, desc.pipeline.vertex);
  return result_ok<asset_data>(a);
}

result<asset_data> loader_load_font(pyro::memory::arena &arena, const asset_descriptor &desc) {
  ZoneScopedN("Pipeline Load");
  asset_data a;

  a.type = ASSET_TYPE_FONT;
  a.font.file_data = load_binary_file(arena, desc.font.path, a.font.file_len);

  return result_ok(a);
}

void process_thread(pyro::memory::arena &arena, const asset_set &set, async_load_result *res) {
  ZoneScopedN("Load Proces Thread");
  while (1) {
    size_t i = res->i.fetch_add(1);
    if (i >= res->count)
      break;

    result<asset_data> data;
    switch (set.descriptors[i].type) {
    case ASSET_TYPE_TEXTURE:
      data = loader_load_texture(arena, set.descriptors[i]);
      break;
    case ASSET_TYPE_PIPELINE:
      data = loader_load_pipeline(arena, set.descriptors[i]);
      break;
    case ASSET_TYPE_FONT:
      data = loader_load_font(arena, set.descriptors[i]);
      break;
    default:
      res->error = "Could not load asset";
      res->has_error = true;
      break;
    }

    if (!data.ok()) {
      res->error = "Could not load asset";
      res->has_error = true;
    }

    res->result.set[i] = data;
    res->result.set[i].name = set.descriptors[i].name;
    res->loaded++;
  }
}

async_load_result *asset_loader_load_async(pyro::memory::arena &arena,
                                           app_state *state,
                                           const asset_set &set) {
  ZoneScopedN("Async Asset Load Begin");

  async_load_result *res = arena.push<async_load_result>();
  res->count = set.descriptors.size();
  res->loaded = 0;
  res->i = 0;
  res->has_error = false;

  // allocate the asset data in the result
  res->result.set.lt_init(arena, set.descriptors.size());

  {
    ZoneScopedN("Start Loader Threads");
    int hw_threads = std::thread::hardware_concurrency();

    res->loader_thread_count =
        std::min(std::max((int)hw_threads - 2, (int)1), MAX_NUM_LOADER_THREADS);

    for (int i = 0; i < res->loader_thread_count; i++) {
      new(&res->loader_threads[i]) std::thread(process_thread, std::ref(arena), std::cref(set), res);
    }

    return res;
  }
}

APIFUNC result<async_load_result_info> asset_loader_async_query(async_load_result &result) {
  if (result.has_error)
    return result_err<async_load_result_info>(result.error.c_str());
  else
    return result_ok(async_load_result_info{result.count, result.loaded.load()});
}

result<> asset_loader_upload_to_vram(app_state *state, async_load_result *result) {
  ZoneScopedN("Upload to VRAM");
  {
    ZoneScopedN("Join loader threads");

    for (int i = 0; i < result->loader_thread_count; i++) {
      if (result->loader_threads[i].joinable())
        result->loader_threads[i].join();
    }
  }

  if (result->has_error)
    return result_err(result->error.c_str());

  for (int i = 0; i < result->count; i++) {
    switch (result->result.set[i].type) {
    case ASSET_TYPE_TEXTURE:
      asset_texture_create(state, pyro::container::string_id(result->result.set[i].name), &result->result.set[i].texture);
      break;
    case ASSET_TYPE_PIPELINE:
      asset_pipeline_create(state, pyro::container::string_id(result->result.set[i].name), &result->result.set[i].pipeline);
      break;
    case ASSET_TYPE_FONT:
      asset_font_create(state, pyro::container::string_id(result->result.set[i].name), &result->result.set[i].font);
      break;
    default:
      return result_err("Unknown asset type");
      break;
    }
  }
  return result_ok(true);
}

result<> asset_loader_load_file_sync(app_state *state, const char *filename) {
  ZoneScopedN("Load file Sync");
  mem_scratch_arena arena = arena_scratch_get();

  result_forward_err(set, asset_set_load_from_file(arena, filename));

  async_load_result *res = asset_loader_load_async(arena, state, set);

  result_forward_err(_, asset_loader_upload_to_vram(state, res));

  arena_scratch_free(arena);

  return result_ok(true);
}

result<async_load_result *> asset_loader_load_file_async(pyro::memory::arena &arena,
                                                         app_state *state,
                                                         const char *filename) {
  ZoneScopedN("Load file Async start");
  result_forward_err(set, asset_set_load_from_file(arena, filename));
  async_load_result *res = asset_loader_load_async(arena, state, set);

  return result_ok(res);
}
