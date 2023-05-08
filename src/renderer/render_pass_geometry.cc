#include "renderer/render_pass_geometry.hh"
#include "assetmanager/assetmanager.hh"
#include "common/hash.hh"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

static void draw_star(app_state *state, glm::vec2 pos, float raduis, glm::vec3 color) {
  glm::mat4 model =
      glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(pos - glm::vec2(raduis), 0.f)),
                 glm::vec3(raduis * 2.0));

  glm::mat4 mvp = state->game.renderer.camp * state->game.renderer.camv * model;

  renderer_mesh m = asset_mesh_get_render(state, HASH_KEY("Quad"));

  renderer_mesh *mp = &m;

  renderer_pipeline p = asset_pipeline_get_render(state, HASH_KEY("solar"));

  pipeline_settings settings = pipeline_settings_create(p, state->frame_arena);
  pipeline_settings_set_uniform(settings, 0, mvp);
  pipeline_settings_set_uniform(settings, 1, asset_texture_get_render(state, HASH_KEY("organic1")));
  pipeline_settings_set_uniform(settings, 2, (float)state->time.t);
  pipeline_settings_set_uniform(settings, 3, color);

  pipeline_settings *pp = &settings;
  state->api.renderer.draw_meshes(1, &mp, &pp, &p);
}

void render_pass_geometry(app_state *state) {
  state->api.renderer.use_framebuffer(state->game.renderer.bloom_buffers[0]);
  state->api.renderer.set_viewport(state->window_area.w, state->window_area.h);
  glm::vec3 cc = state->game.renderer.clear_color;
  state->api.renderer.clear(cc.x, cc.y, cc.z, 1.0);

  draw_star(state,
            state->game.solar_system->star.pos,
            state->game.solar_system->star.raduis,
            state->game.solar_system->star.color);
}
