#include "assetmanager.hh"
#include "common/memory_pool.hh"
#include <tracy/Tracy.hpp>

#define create_function(name)                                                                      \
  uint32_t asset_##name##_create(app_state *state, uint32_t bundle, name##_data *data) {           \
    asset_bundle *b = pool_get_at_index(state->assets.bundles, bundle);                            \
    renderer_##name t = state->api.renderer.create_##name(data);                                   \
    renderer_##name *k = pool_alloc(b->name##_data);                                               \
    *k = t;                                                                                        \
    return pool_get_index(b->name##_data, k);                                                      \
  }

#define update_function(name)                                                                      \
  uint32_t asset_##name##_update(app_state *state,                                                 \
                                 uint32_t bundle,                                                  \
                                 uint32_t id,                                                      \
                                 name##_data *data) {                                              \
    asset_bundle *b = pool_get_at_index(state->assets.bundles, bundle);                            \
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

#define get_function(name)                                                                         \
  renderer_##name asset_##name##_get_render(app_state *state, uint32_t bundle, uint32_t id) {    \
    asset_bundle *b = pool_get_at_index(state->assets.bundles, bundle);                            \
    renderer_##name *r = pool_get_at_index(b->name##_data, id);                                    \
    return *r;                                                                                     \
  }

void setup_default_bundle(app_state *state) {
  asset_bundle_create(state, "internal");
}

void asset_system_init(app_state *state) {
  ZoneScopedN("Init Asset System");
  state->assets.bundles = pool_create<asset_bundle>(1024);
  setup_default_bundle(state);
}

void asset_system_shutdown(app_state *state) {
  pool_free(state->assets.bundles);
}

uint32_t asset_bundle_create(app_state *state, const char *name) {
  asset_bundle *b = pool_alloc(state->assets.bundles);
  b->texture_data = pool_create<renderer_texture>(4096);
  b->pipeline_data = pool_create<renderer_pipeline>(4096);
  b->mesh_data = pool_create<renderer_mesh>(4096);

  return pool_get_index(state->assets.bundles, b);
}

void asset_bundle_free(app_state *state, uint32_t bundle) {
  asset_bundle *b = pool_get_at_index(state->assets.bundles, bundle);
  // FIXME this leaks gpu resources.
  pool_free(b->texture_data);
  pool_free(b->pipeline_data);
  pool_free(b->mesh_data);

  pool_pop(state->assets.bundles, b);
}

create_function(texture);
create_function(pipeline);
create_function(mesh);

update_function(texture);
update_function(mesh);

delete_function(texture);
delete_function(pipeline);
delete_function(mesh);

get_function(texture);
get_function(pipeline);
get_function(mesh);
