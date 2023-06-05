#include "assetmanager.hh"
#include "common/hash.hh"
#include "common/result.hh"
#include "data/asset_storage.hh"
#include "data/asset_types.hh"
#include "renderer/text/render_text.hh"
#include "tracy/Tracy.hpp"

#define create_function(name, ENUM_NAME)                                                           \
  void asset_##name##_create(app_state *state, const char *id, name##_data *data) {                \
    ZoneScopedN("Create " #name);                                                                  \
    asset_##name##_delete(state, id);                                                              \
    renderer_##name t = state->api.renderer.create_##name(data);                                   \
    renderer_##name *k = state->assets.name##_data.alloc();                                        \
    *k = t;                                                                                        \
    state->assets.asset_lookup.insert(id, asset_index{ENUM_NAME, (void *)k});                      \
  }

#define update_function(name, ENUM_NAME)                                                           \
  void asset_##name##_update(app_state *state, const char *id, name##_data *data) {                \
    ZoneScopedN("Update " #name);                                                                  \
    auto i = state->assets.asset_lookup.find(id);                                                  \
    if (i != state->assets.asset_lookup.end() && i->val.type == ENUM_NAME) {                       \
      state->api.renderer.update_##name((renderer_##name *)i->val.value, data);                    \
    }                                                                                              \
  }

#define delete_function(name, ENUM_NAME)                                                           \
  void asset_##name##_delete(app_state *state, const char *id) {                                   \
    ZoneScopedN("Delete " #name);                                                                  \
    auto i = state->assets.asset_lookup.find(id);                                                  \
    if (i != state->assets.asset_lookup.end() && i->val.type == ENUM_NAME) {                       \
      state->api.renderer.delete_##name(*(renderer_##name *)i->val.value);                         \
      state->assets.name##_data.free((renderer_##name *)i->val.value);                             \
      state->assets.asset_lookup.erase(id);                                                        \
    }                                                                                              \
  }

#define get_function(name, ENUM_NAME)                                                              \
  result<renderer_##name> asset_##name##_get_render(app_state *state, const char *id) {            \
    ZoneScopedN("Get " #name);                                                                     \
    auto i = state->assets.asset_lookup.find(id);                                                  \
    if (i != state->assets.asset_lookup.end() && i->val.type == ENUM_NAME)                         \
      return result_ok(*(renderer_##name *)i->val.value);                                          \
    return result_err<renderer_##name>("Could not find " #name " %s", id);                         \
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

  state->assets.texture_data.lt_init(1024);
  state->assets.pipeline_data.lt_init(1024);
  state->assets.mesh_data.lt_init(1024);
  state->assets.framebuffer_data.lt_init(1024);
  state->assets.font_data.lt_init(32);

  state->assets.asset_lookup.lt_init(1024);

  init_default_assets(state);
}

void asset_system_shutdown(app_state *state) {
  state->assets.texture_data.lt_done();
  state->assets.mesh_data.lt_done();
  state->assets.framebuffer_data.lt_done();
  state->assets.pipeline_data.lt_done();
  state->assets.font_data.lt_done();
  state->assets.asset_lookup.lt_done();
}

create_function(texture, ASSET_TYPE_TEXTURE);
create_function(pipeline, ASSET_TYPE_PIPELINE);
create_function(mesh, ASSET_TYPE_MESH);
create_function(framebuffer, ASSET_TYPE_FRAMEBUFFER);

void asset_font_create(app_state *state, const char *id, font_data *data) {
  ZoneScopedN("Create Font");
  asset_font_delete(state, id);
  renderer_font t = render_font_create(state, data);
  renderer_font *k = state->assets.font_data.alloc();
  *k = t;
  state->assets.asset_lookup.insert(id, asset_index{ASSET_TYPE_FONT, (void *)k});
};

update_function(texture, ASSET_TYPE_TEXTURE);
update_function(mesh, ASSET_TYPE_MESH);

delete_function(texture, ASSET_TYPE_TEXTURE);
delete_function(pipeline, ASSET_TYPE_PIPELINE);
delete_function(mesh, ASSET_TYPE_MESH);
delete_function(framebuffer, ASSET_TYPE_FRAMEBUFFER);

void asset_font_delete(app_state *state, const char *id) {
  ZoneScopedN("Delete Font");

  auto i = state->assets.asset_lookup.find(id);
  if (i != state->assets.asset_lookup.end() && i->val.type == ASSET_TYPE_FONT) {
    render_font_delete(state, *(renderer_font *)i->val.value);
    state->assets.font_data.free((renderer_font *)i->val.value);
    state->assets.asset_lookup.erase(id);
  }
};

get_function(texture, ASSET_TYPE_TEXTURE);
get_function(pipeline, ASSET_TYPE_PIPELINE);
get_function(mesh, ASSET_TYPE_MESH);
get_function(framebuffer, ASSET_TYPE_FRAMEBUFFER);
get_function(font, ASSET_TYPE_FONT);
