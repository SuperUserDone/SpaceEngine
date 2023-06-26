#include "assetmanager.hh"
#include "common/hash.hh"
#include "common/result.hh"
#include "data/asset_storage.hh"
#include "data/asset_types.hh"
#include "renderer/text/render_text.hh"
#include "tracy/Tracy.hpp"

#define create_function(name)                                                                      \
  void asset_##name##_create(app_state *state,                                                     \
                             const pyro::container::string_id &id,                                 \
                             name##_data *data) {                                                  \
    ZoneScopedN("Create " #name);                                                                  \
    asset_##name##_delete(state, id);                                                              \
    renderer_##name t = state->api.renderer.create_##name(data);                                   \
    state->assets.name##_data.insert(id, t);                                                       \
  }

#define update_function(name)                                                                      \
  void asset_##name##_update(app_state *state,                                                     \
                             const pyro::container::string_id &id,                                 \
                             name##_data *data) {                                                  \
    ZoneScopedN("Update " #name);                                                                  \
    auto i = state->assets.name##_data.find(id);                                                   \
    if (i != state->assets.name##_data.end()) {                                                    \
      state->api.renderer.update_##name(&i->val, data);                                            \
    }                                                                                              \
  }

#define delete_function(name)                                                                      \
  void asset_##name##_delete(app_state *state, const pyro::container::string_id &id) {             \
    ZoneScopedN("Delete " #name);                                                                  \
    auto i = state->assets.name##_data.find(id);                                                   \
    if (i != state->assets.name##_data.end()) {                                                    \
      state->api.renderer.delete_##name(i->val);                                                   \
      state->assets.name##_data.erase(id);                                                         \
    }                                                                                              \
  }

#define get_function(name)                                                                         \
  result<renderer_##name> asset_##name##_get_render(app_state *state,                              \
                                                    const pyro::container::string_id &id) {        \
    ZoneScopedN("Get " #name);                                                                     \
    auto i = state->assets.name##_data.find(id);                                                   \
    if (i != state->assets.name##_data.end())                                                      \
      return result_ok(i->val);                                                                    \
    return result_err<renderer_##name>("Could not find " #name " with id %s", id.string());        \
  }

static void init_default_assets(app_state *state) {
  // Quad
  {
    mesh_data m;
    pyro::math::quad q({0.f, 0.f}, {1, 1}, {0.f, 0.f}, {1.f, 1.f});
    m.verticies.lt_init(q.v, 4);
    uint16_t i[] = {0, 1, 2, 0, 2, 3};
    m.indicies.lt_init(i, 6);

    asset_mesh_create(state, "Quad"_sid, &m);
  }
}

void asset_system_init(app_state *state) {
  ZoneScopedN("Init Asset System");

  state->assets.texture_data.lt_init(1024);
  state->assets.pipeline_data.lt_init(1024);
  state->assets.mesh_data.lt_init(1024);
  state->assets.framebuffer_data.lt_init(1024);
  state->assets.font_data.lt_init(64);

  init_default_assets(state);
}

void asset_system_shutdown(app_state *state) {

  for (auto &t : state->assets.texture_data)
    state->api.renderer.delete_texture(t.val);
  for (auto &t : state->assets.mesh_data)
    state->api.renderer.delete_mesh(t.val);
  for (auto &t : state->assets.framebuffer_data)
    state->api.renderer.delete_framebuffer(t.val);
  for (auto &t : state->assets.pipeline_data)
    state->api.renderer.delete_pipeline(t.val);
  for (auto &t : state->assets.font_data)
    render_font_delete(state, t.val);

  state->assets.mesh_data.lt_done();
  state->assets.texture_data.lt_done();
  state->assets.framebuffer_data.lt_done();
  state->assets.pipeline_data.lt_done();
  state->assets.font_data.lt_done();
}

create_function(texture);
create_function(pipeline);
create_function(mesh);
create_function(framebuffer);

void asset_font_create(app_state *state, const pyro::container::string_id &id, font_data *data) {
  ZoneScopedN("Create Font");
  asset_font_delete(state, id);
  renderer_font t = render_font_create(state, data);
  state->assets.font_data.insert(id, t);
};

update_function(texture);
update_function(mesh);

delete_function(texture);
delete_function(pipeline);
delete_function(mesh);
delete_function(framebuffer);

void asset_font_delete(app_state *state, const pyro::container::string_id &id) {
  ZoneScopedN("Delete Font");

  auto i = state->assets.font_data.find(id);
  if (i != state->assets.font_data.end()) {
    render_font_delete(state, i->val);
    state->assets.font_data.erase(id);
  }
};

get_function(texture);
get_function(pipeline);
get_function(mesh);
get_function(framebuffer);
get_function(font);
