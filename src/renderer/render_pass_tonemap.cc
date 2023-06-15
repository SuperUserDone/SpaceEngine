#include "render_pass_tonemap.hh"
#include "assetmanager/assetmanager.hh"
#include "common/hash.hh"
#include "render_utils.hh"
#include "tracy/Tracy.hpp"

void render_pass_tonemap(app_state *state) {
  ZoneScopedN("Renderer Tonemap");
  state->api.renderer.use_default_framebuffer();
  state->api.renderer.set_viewport(state->window_area.w, state->window_area.h);
  state->api.renderer.clear(0.0, 0.0, 0.0, 1.0);
  render_fullscreen_quad(state,
                         asset_pipeline_get_render(state, "tonemap"_sid),
                         state->game.renderer.bloom_buffer_textures[0]);
}
