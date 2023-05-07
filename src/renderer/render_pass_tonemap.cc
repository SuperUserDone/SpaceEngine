#include "render_pass_tonemap.hh"
#include "assetmanager/assetmanager.hh"
#include "common/hash.hh"
#include "render_utils.hh"

void render_pass_tonemap(app_state *state) {
  state->api.renderer.use_default_framebuffer();
  state->api.renderer.set_viewport(state->window_area.w, state->window_area.h);
  state->api.renderer.clear(0.0, 0.0, 0.0, 1.0);
  render_fullscreen_quad(state,
                         asset_pipeline_get_render(state, HASH_KEY("Tonemap")),
                         state->game.renderer.bloom_buffer_textures[0]);
}
