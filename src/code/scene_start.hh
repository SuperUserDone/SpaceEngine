#pragma once

#include "assetmanager/assetmanager.hh"
#include "data/app_state.hh"
#include "renderer/text/render_text.hh"

namespace start {
static inline void scene_tick(app_state *state) {
  if (state->raw_input.mouse_keyboard.keys[input::keyboard::KEY_SPACE]) {
    switch_season(state->game.gameplay, season_type::SUMMER);
  }
}

static inline void scene_text_render(app_state *state) {
  renderer_font font = asset_font_get_render(state, "default"_sid);
  render_text(state, font, 22, {148, 480}, "Press SPACE to start...");
}

static inline void scene_game_render(app_state *state) {

}

} // namespace start