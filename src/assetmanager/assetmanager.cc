#include "assetmanager.hh"
#include "common/hash.hh"
#include "common/result.hh"
#include "tracy/Tracy.hpp"

#define create_function(name, ENUM_NAME)                                                           \
  void asset_##name##_create(app_state *state, const char *id, name##_data *data) {                \
    ZoneScopedN("Create " #name);                                                                  \
    asset_##name##_delete(state, id);                                                              \
    renderer_##name t = state->api.renderer.create_##name(data);                                   \
    renderer_##name *k = pool_alloc(state->assets.name##_data);                                    \
    *k = t;                                                                                        \
    asset_index *i = pool_alloc(state->assets.index_table);                                        \
    *i = asset_index{ENUM_NAME, (void *)k};                                                        \
    hash_table_insert(state->assets.asset_lookup, id, (void *)i);                                  \
  }

#define update_function(name, ENUM_NAME)                                                           \
  void asset_##name##_update(app_state *state, const char *id, name##_data *data) {                \
    ZoneScopedN("Update " #name);                                                                  \
    asset_index *i = _asset_table_find(state, id);                                                 \
    if (i && i->type == ENUM_NAME) {                                                               \
      state->api.renderer.update_##name((renderer_##name *)i->value, data);                        \
    }                                                                                              \
  }

#define delete_function(name, ENUM_NAME)                                                           \
  void asset_##name##_delete(app_state *state, const char *id) {                                   \
    ZoneScopedN("Delete " #name);                                                                  \
    asset_index *i = _asset_table_find(state, id);                                                 \
    if (i && i->type == ENUM_NAME) {                                                               \
      state->api.renderer.delete_##name(*(renderer_##name *)i->value);                             \
      pool_pop(state->assets.name##_data, (renderer_##name *)i->value);                            \
      pool_pop(state->assets.index_table, i);                                                      \
      hash_table_delete(state->assets.asset_lookup, id);                                           \
    }                                                                                              \
  }

#define get_function(name, ENUM_NAME)                                                              \
  result<renderer_##name> asset_##name##_get_render(app_state *state, const char *id) {            \
    ZoneScopedN("Get " #name);                                                                     \
    asset_index *i = _asset_table_find(state, id);                                                 \
    if (i && i->type == ENUM_NAME)                                                                 \
      return result_ok(*(renderer_##name *)i->value);                                              \
    return result_err<renderer_##name>("Could not find " #name " %s", id);                         \
  }

asset_index *_asset_table_find(app_state *state, const char *id) {
  return (asset_index *)hash_table_search(state->assets.asset_lookup, id);
}

static void init_default_assets(app_state *state) {
  // Quad
  {
    mesh_data m;
    vertex v[] = {vertex{{0.f, 0.f}, {0.f, 0.f}},
                  vertex{{1.f, 0.f}, {1.f, 0.f}},
                  vertex{{1.f, 1.f}, {1.f, 1.f}},
                  vertex{{0.f, 1.f}, {0.f, 1.f}}};
    m.verticies = v;
    uint32_t i[] = {0, 1, 2, 0, 2, 3};
    m.indicies = i;

    m.index_count = 6;
    m.vertex_count = 4;

    asset_mesh_create(state, "Quad", &m);
  }
}

void asset_system_init(app_state *state) {
  ZoneScopedN("Init Asset System");

  state->assets.texture_data = pool_create<renderer_texture>(1024);
  state->assets.pipeline_data = pool_create<renderer_pipeline>(1024);
  state->assets.mesh_data = pool_create<renderer_mesh>(1024);
  state->assets.framebuffer_data = pool_create<renderer_framebuffer>(1024);

  state->assets.asset_lookup = hash_table_create(state->permanent_arena);
  state->assets.index_table = pool_create<asset_index>(1024);

  init_default_assets(state);
}

void asset_system_shutdown(app_state *state) {
  pool_free(state->assets.texture_data);
  pool_free(state->assets.mesh_data);
  pool_free(state->assets.framebuffer_data);
  pool_free(state->assets.pipeline_data);
  pool_free(state->assets.index_table);
}

create_function(texture, ASSET_TYPE_TEXTURE);
create_function(pipeline, ASSET_TYPE_PIPELINE);
create_function(mesh, ASSET_TYPE_MESH);
create_function(framebuffer, ASSET_TYPE_FRAMEBUFFER);

update_function(texture, ASSET_TYPE_TEXTURE);
update_function(mesh, ASSET_TYPE_MESH);

delete_function(texture, ASSET_TYPE_TEXTURE);
delete_function(pipeline, ASSET_TYPE_PIPELINE);
delete_function(mesh, ASSET_TYPE_MESH);
delete_function(framebuffer, ASSET_TYPE_FRAMEBUFFER);

get_function(texture, ASSET_TYPE_TEXTURE);
get_function(pipeline, ASSET_TYPE_PIPELINE);
get_function(mesh, ASSET_TYPE_MESH);
get_function(framebuffer, ASSET_TYPE_FRAMEBUFFER);
