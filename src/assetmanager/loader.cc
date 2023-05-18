#include "loader.hh"
#include "assetmanager/asset_set.hh"
#include "assetmanager/assetmanager.hh"
#include "common/debug.hh"
#include "common/file_utils.hh"
#include "common/hash.hh"
#include "common/memory_arena.hh"
#include "common/result.hh"
#include "data/asset_storage.hh"
#include "data/asset_types.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

result<asset_data> loader_load_texture(mem_arena &arena, const asset_descriptor &desc) {
  int x, y, c;
  void *data = stbi_load(desc.texture.file_path, &x, &y, &c, 0);

  if (!data)
    return result_err<asset_data>("Could not load file %s from disk", desc.texture.file_path);

  void *new_data = arena_push(arena, x * y * c);
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

result<asset_data> loader_load_pipeline(mem_arena &arena, const asset_descriptor &desc) {
  asset_data a;
  a.type = ASSET_TYPE_PIPELINE;
  a.pipeline.uniform_count = desc.pipeline.uniforms_count;
  // Set needs to remain in scope until asset are uploaded to GPU. I might change this later.
  a.pipeline.uniform_names = (const char **)desc.pipeline.uniforms;
  a.pipeline.fragment_shader = load_file(arena, desc.pipeline.fragment);
  a.pipeline.vertex_shader = load_file(arena, desc.pipeline.vertex);
  return result_ok<asset_data>(a);
}

load_result *asset_loader_load_sync(mem_arena &arena, app_state *state, const asset_set &set) {
  load_result *res = arena_push_struct(arena, load_result);
  res->count = set.count;
  res->semaphore = 1;

  res->set = arena_push_array(arena, asset_data, set.count);

  for (size_t i = 0; i < set.count; i++) {
    switch (set.descriptors[i].type) {
    case ASSET_TYPE_TEXTURE:
      res->set[i] = loader_load_texture(arena, set.descriptors[i]);
      break;
    case ASSET_TYPE_PIPELINE:
      res->set[i] = loader_load_pipeline(arena, set.descriptors[i]);
      break;
    default:
      // Should not happen
      break;
    }

    // TODO investigate copy so set can be deleted after this function.
    res->set[i].name = set.descriptors[i].name;
  }

  res->semaphore--;
  return res;
}

void asset_loader_upload_to_vram(app_state *state, load_result *result) {
  while (result->semaphore)
    _mm_pause(); // Spin until available

  for (int i = 0; i < result->count; i++) {
    switch (result->set[i].type) {
    case ASSET_TYPE_TEXTURE:
      asset_texture_create(state, HASH_KEY(result->set[i].name), &result->set[i].texture);
      break;
    case ASSET_TYPE_PIPELINE:
      asset_pipeline_create(state, HASH_KEY(result->set[i].name), &result->set[i].pipeline);
      break;
    default:
      break;
    }
  }
}
