#include "renderer/render_pass_geometry.hh"
#include "assetmanager/assetmanager.hh"
#include "common/hash.hh"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

static void draw_star(app_state *state, glm::vec2 pos, float raduis) {
  glm::vec2 area = {(float)state->window_area.w, (float)state->window_area.h};
  area /= state->window_area.dpi_scaling;
  area /= state->game.camera.zoom;

  glm::mat4 cam =
      glm::ortho(-area.x, area.x, -area.y, area.y, -1.f, 1.f) *
      glm::scale(glm::translate(glm::mat4(1.f),
                                glm::vec3(pos - glm::vec2(raduis) - state->game.camera.pos, 0.f)),
                 glm::vec3(raduis * 2.0));

  renderer_mesh m = asset_mesh_get_render(state, HASH_KEY("Quad"));

  renderer_mesh *mp = &m;

  renderer_pipeline p = asset_pipeline_get_render(state, HASH_KEY("solar"));

  pipeline_settings settings = pipeline_settings_create(p, state->frame_arena);
  pipeline_settings_set_uniform(settings, 0, cam);
  pipeline_settings_set_uniform(settings, 1, asset_texture_get_render(state, HASH_KEY("organic1")));
  pipeline_settings_set_uniform(settings, 2, (float)state->time.t);
  pipeline_settings_set_uniform(settings, 3, state->game.sun_color);

  pipeline_settings *pp = &settings;
  state->api.renderer.draw_meshes(1, &mp, &pp, &p);
}

void render_pass_geometry(app_state *state) {

  state->api.renderer.use_framebuffer(state->game.renderer.bloom_buffers[0]);
  state->api.renderer.set_viewport(state->window_area.w, state->window_area.h);
  state->api.renderer.clear(0.0, 0.0, 0.0, 1.0);

  draw_star(state, glm::vec2(0.f), 100.f);
}
