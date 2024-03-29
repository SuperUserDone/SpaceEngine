#include "renderer.hh"
#include "renderer/render_pass_bloom.hh"
#include "renderer/render_pass_geometry.hh"
#include "renderer/render_pass_tonemap.hh"
#include "tracy/Tracy.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

void render_init(app_state *state) {
  render_pass_bloom_init(state);
  state->api.renderer.set_blending(BLEND_ONE_MIN_SRC_ALPHA);
}

void render_resize(app_state *state, int x, int y) {
  render_pass_bloom_resize(state);
}

void render_calculate_camera(app_state *state) {
  glm::vec2 area = {(float)state->window_area.w, (float)state->window_area.h};
  area /= state->window_area.dpi_scaling;
  area /= (state->game.camera.zoom * 100.f);

  state->game.renderer.camp = glm::ortho(-area.x, area.x, area.y, -area.y, -1.f, 1.f);
  state->game.renderer.camv =
      glm::translate(glm::mat4(1.f), glm::vec3(-state->game.camera.pos, 0.f));
}

void render_game(app_state *state) {
  ZoneScopedN("Game Render");
  render_calculate_camera(state);
  render_pass_geometry(state);
  render_pass_bloom(state);
  render_pass_tonemap(state);
}
