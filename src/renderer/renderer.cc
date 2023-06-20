#include "renderer.hh"
#include "renderer/render_pass_bloom.hh"
#include "renderer/render_pass_geometry.hh"
#include "renderer/render_pass_tonemap.hh"
#include "tracy/Tracy.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#define _USE_MATH_DEFINES

#include <math.h>
#include "PerlinNoise.hpp"

void render_init(app_state *state) {
  render_pass_bloom_init(state);
  state->api.renderer.set_blending(BLEND_ONE_MIN_SRC_ALPHA);
}

void render_resize(app_state *state, int x, int y) {
  render_pass_bloom_resize(state);
}

	const siv::PerlinNoise::seed_type seed = 123456u;

	const siv::PerlinNoise perlin{ seed };

glm::vec2 shake_fn(float x) {
  return glm::vec2(perlin.noise1D(x), perlin.noise2D(0, x));
}

void render_calculate_camera(app_state *state) {
  glm::vec2 area = {(float)state->window_area.w, (float)state->window_area.h};
  area /= state->window_area.dpi_scaling;
  area /= (state->game.camera.zoom * 1.f);

  state->game.renderer.camp = glm::ortho(0.f, area.x, area.y, 0.f, -1.f, 1.f);
  state->game.renderer.camv =
      glm::translate(glm::mat4(1.f),
                     glm::vec3(-state->game.camera.pos, 0.f) +
                         glm::vec3(shake_fn(state->time.t * 200) * std::min(state->game.camera.shake, 50.f), 0.f));
}

void render_game(app_state *state) {
  ZoneScopedN("Game Render");
  render_calculate_camera(state);
  render_pass_geometry(state);
  render_pass_bloom(state);
  render_pass_tonemap(state);
}
