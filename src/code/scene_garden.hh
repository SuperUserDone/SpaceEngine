#pragma once
#include "assetmanager/assetmanager.hh"
#include "code/player.hh"
#include "code/scene_garden.hh"
#include "data/app_state.hh"
#include "renderer/text/render_text.hh"

namespace garden {

static inline void scene_tick(app_state *state) {
  const float player_speed = 256;

  if (state->raw_input.mouse_keyboard.keys[input::keyboard::KEY_W]) {
    state->game.gameplay.player_pos += glm::vec2(0.f, -player_speed * state->time.dt);
  }
  if (state->raw_input.mouse_keyboard.keys[input::keyboard::KEY_S]) {
    state->game.gameplay.player_pos += glm::vec2(0.f, player_speed * state->time.dt);
  }
  if (state->raw_input.mouse_keyboard.keys[input::keyboard::KEY_A]) {
    state->game.gameplay.player_pos += glm::vec2(-player_speed * state->time.dt, 0.f);
  }
  if (state->raw_input.mouse_keyboard.keys[input::keyboard::KEY_D]) {
    state->game.gameplay.player_pos += glm::vec2(player_speed * state->time.dt, 0.f);
  }

  if (state->game.gameplay.player_pos.x < 0)
    state->game.gameplay.player_pos.x = 0;
  if (state->game.gameplay.player_pos.y < 0)
    state->game.gameplay.scene = scene_type::FOREST;

  if (state->game.gameplay.player_pos.x + 32 > 512 / state->game.camera.zoom)
    state->game.gameplay.player_pos.x = 512 / state->game.camera.zoom - 32;
  if (state->game.gameplay.player_pos.y + 32 > 512 / state->game.camera.zoom)
    state->game.gameplay.player_pos.y = 512 / state->game.camera.zoom - 32;
}

static inline void scene_text_render(app_state *state) {
  renderer_font font = asset_font_get_render(state, "default"_sid);
  render_text(state, font, 24, {8, 32}, "Garden");
}

static inline void scene_game_render(app_state *state) {
  render_player(state);
}

} // namespace garden