#pragma once

#include "assetmanager/assetmanager.hh"
#include "data/app_state.hh"
#include "renderer/render_batch.hh"
#include <glm/ext/matrix_clip_space.hpp>

static inline void render_sprites(app_state *state) {
  glm::vec2 area = {(float)state->window_area.w, (float)state->window_area.h};
  area /= state->window_area.dpi_scaling;

  auto p = asset_pipeline_get_render(state, "sprite"_sid).get_no_err();
  auto spritesheet = asset_texture_get_render(state, "spritesheet"_sid).get_no_err();
  auto s = pipeline_settings_create(p, state->frame_arena);

  pipeline_settings_set_uniform(s, 0, state->game.renderer.camp * state->game.renderer.camv);
  pipeline_settings_set_uniform(s, 1, spritesheet);

  render_batch_render(state, state->game.b, p, s);
  render_batch_reset(state->game.b);
}
