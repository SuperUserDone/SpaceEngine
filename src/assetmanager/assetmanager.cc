#include "assetmanager.hh"
#include "common/hash_table.hh"
#include "common/memory_arena.hh"
#include "common/memory_pool.hh"
#include "data/asset_storage.hh"
#include "data/asset_types.hh"
#include <tracy/Tracy.hpp>

#define create_function(name, ENUM_NAME)                                                           \
  void asset_##name##_create(app_state *state, size_t id, name##_data *data) {                     \
    renderer_##name t = state->api.renderer.create_##name(data);                                   \
    renderer_##name *k = pool_alloc(state->assets.name##_data);                                    \
    *k = t;                                                                                        \
    asset_index *i = pool_alloc(state->assets.index_table);                                        \
    *i = {ENUM_NAME, pool_get_index(state->assets.name##_data, k)};                                \
    hash_table_insert(state->assets.asset_lookup, id, (void *)i);                                  \
  }

#define update_function(name)                                                                      \
  uint32_t asset_##name##_update(app_state *state, uint32_t id, name##_data *data) {               \
    renderer_##name *r = pool_get_at_index(b->name##_data, id);                                    \
    state->api.renderer.update_##name(r, data);                                                    \
    return id;                                                                                     \
  }

#define delete_function(name)                                                                      \
  void asset_##name##_delete(app_state *state, uint32_t bundle, uint32_t id) {                     \
    asset_bundle *b = pool_get_at_index(state->assets.bundles, bundle);                            \
    renderer_##name *r = pool_get_at_index(b->name##_data, id);                                    \
    state->api.renderer.delete_##name(*r);                                                         \
  }

#define get_function(name, ENUM_TYPE)                                                              \
  renderer_##name asset_##name##_get_render(app_state *state, size_t id) {                         \
    asset_index *i = _asset_table_find(state, id);                                                 \
    if (i && i->type == ENUM_TYPE)                                                                 \
      return *pool_get_at_index(state->assets.name##_data, i->lookup_value);                                    \
    return renderer_##name{};                                                                      \
  }

asset_index *_asset_table_find(app_state *state, size_t id) {
  return (asset_index *)hash_table_search(state->assets.asset_lookup, id);
}

void asset_system_init(app_state *state) {
  ZoneScopedN("Init Asset System");

  state->assets.texture_data = pool_create<renderer_texture>(1024);
  state->assets.pipeline_data = pool_create<renderer_pipeline>(1024);
  state->assets.mesh_data = pool_create<renderer_mesh>(1024);

  state->assets.asset_lookup = hash_table_create(state->permanent_arena);
  state->assets.index_table = pool_create<asset_index>(1024);
}

void asset_system_shutdown(app_state *state) {
  pool_free(state->assets.texture_data);
  pool_free(state->assets.mesh_data);
  pool_free(state->assets.pipeline_data);
  pool_free(state->assets.index_table);
}

create_function(texture, ASSET_TYPE_TEXTURE);
create_function(pipeline, ASSET_TYPE_PIPELINE);
create_function(mesh, ASSET_TYPE_MESH);
/*
update_function(texture);
update_function(mesh);

delete_function(texture);
delete_function(pipeline);
delete_function(mesh);
*/
get_function(texture, ASSET_TYPE_TEXTURE);
get_function(pipeline, ASSET_TYPE_PIPELINE);
get_function(mesh, ASSET_TYPE_MESH);
