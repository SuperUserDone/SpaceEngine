#pragma once

#include "assetmanager/assetmanager.hh"
#include "data/app_state.hh"
#include "renderer/text/render_text.hh"
namespace anim {

static inline void scene_tick(app_state *state) {
  state->game.gameplay.anim_progress += state->time.dt;
  if(state->game.gameplay.anim_len < state->game.gameplay.anim_progress) {
    state->game.gameplay.scene = scene_type::GARDEN;
  }
}

static inline void scene_text_render(app_state *state) {
  renderer_font font = asset_font_get_render(state, "default"_sid);
  render_text(state, font, 50, {170, 250}, get_seson_name(state->game.gameplay.season));
}

static inline void scene_game_render(app_state *state) {
}

} // namespace anim