#include "renderer/render_utils.hh"
#include "assetmanager/assetmanager.hh"
#include "common/hash.hh"
#include <glm/ext/matrix_clip_space.hpp>

void render_fullscreen_quad(app_state *state, renderer_pipeline p, renderer_texture tex) {
  glm::mat4 mvp = glm::ortho(0.f, 1.f, 0.f, 1.f, -1.f, 1.f);

  renderer_mesh m = asset_mesh_get_render(state, "Quad"_sid);

  pipeline_settings settings = pipeline_settings_create(p, state->frame_arena);
  pipeline_settings_set_uniform(settings, 0, mvp);
  pipeline_settings_set_uniform(settings, 1, tex);

  renderer_mesh *mp = &m;
  pipeline_settings *pp = &settings;

  state->api.renderer.draw_meshes(1, &mp, &pp, &p);
}
