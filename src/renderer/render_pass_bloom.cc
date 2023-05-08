#include "renderer/render_pass_bloom.hh"
#include "assetmanager/assetmanager.hh"
#include "common/hash.hh"
#include <glm/common.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#define min(x, y) (((x) > (y)) ? (y) : (x))

static void bloom_render_downsample(app_state *state) {
  glm::mat4 mvp = glm::ortho(0.f, 1.f, 0.f, 1.f, -1.f, 1.f);

  int32_t firstpass = 1;

  for (int i = 0; i < state->game.renderer.bloom_iters; i++) {

    glm::vec2 window = state->game.renderer.bloom_viewports[i];
    glm::vec2 window_smaller = state->game.renderer.bloom_viewports[i + 1];
    renderer_mesh m = asset_mesh_get_render(state, HASH_KEY("Quad"));

    renderer_mesh *mp = &m;

    state->api.renderer.use_framebuffer(state->game.renderer.bloom_buffers[i + 1]);
    renderer_pipeline p = asset_pipeline_get_render(state, HASH_KEY("bloomds"));
    pipeline_settings settings = pipeline_settings_create(p, state->frame_arena);

    pipeline_settings_set_uniform(settings, 0, mvp);
    pipeline_settings_set_uniform(settings, 1, state->game.renderer.bloom_buffer_textures[i]);
    pipeline_settings_set_uniform(settings, 2, window);
    pipeline_settings_set_uniform(settings, 3, state->game.renderer.bloom_params);
    pipeline_settings_set_uniform(settings, 4, firstpass);

    pipeline_settings *pp = &settings;

    state->api.renderer.set_viewport(window_smaller.x, window_smaller.y);
    state->api.renderer.clear(0.f, 0.f, 0.f, 1.f);
    state->api.renderer.draw_meshes(1, &mp, &pp, &p);

    firstpass = 0;
  }
}

static void bloom_render_upsample(app_state *state) {
  glm::mat4 mvp = glm::ortho(0.f, 1.f, 0.f, 1.f, -1.f, 1.f);

  state->api.renderer.set_blending(BLEND_ADD);

  for (int i = state->game.renderer.bloom_iters - 1; i >= 0; i--) {

    glm::vec2 window = state->game.renderer.bloom_viewports[i];

    renderer_mesh m = asset_mesh_get_render(state, HASH_KEY("Quad"));

    renderer_mesh *mp = &m;

    state->api.renderer.use_framebuffer(state->game.renderer.bloom_buffers[i]);
    renderer_pipeline p = asset_pipeline_get_render(state, HASH_KEY("bloomus"));

    pipeline_settings settings = pipeline_settings_create(p, state->frame_arena);

    pipeline_settings_set_uniform(settings, 0, mvp);
    pipeline_settings_set_uniform(settings, 1, state->game.renderer.bloom_buffer_textures[i + 1]);
    pipeline_settings_set_uniform(settings, 2, state->game.renderer.bloom_size);

    pipeline_settings *pp = &settings;

    state->api.renderer.set_viewport(window.x, window.y);
    state->api.renderer.draw_meshes(1, &mp, &pp, &p);
  }

  state->api.renderer.set_blending(BLEND_ONE_MIN_SRC_ALPHA);
}

void render_pass_bloom(app_state *state) {
  bloom_render_downsample(state);
  bloom_render_upsample(state);
}

void render_pass_bloom_init(app_state *state) {
  glm::uvec2 window = {state->window_area.w, state->window_area.h};
  state->game.renderer.bloom_iters = MAX_BLOOM_ITERATIONS;
  for (int i = 0; i < MAX_BLOOM_ITERATIONS; i++) {

    texture_data t;
    t.w = window.x;
    t.h = window.y;
    t.data = nullptr;
    t.format = TEX_FORMAT_RGBA_F16;
    t.upscale = TEX_FILTER_LINEAR;
    t.downscale = TEX_FILTER_LINEAR;
    t.edge = TEX_EDGE_CLAMP;
    state->game.renderer.bloom_buffer_textures[i] = state->api.renderer.create_texture(&t);

    framebuffer_data fbd;
    fbd.color_attachment = state->game.renderer.bloom_buffer_textures[i];
    state->game.renderer.bloom_buffers[i] = state->api.renderer.create_framebuffer(&fbd);
    state->game.renderer.bloom_viewports[i] = window;

    if (window.x < 10 || window.y < 10) {
      state->game.renderer.bloom_iters = min(i, state->game.renderer.bloom_iters);
    }

    window /= 2;
    window = glm::max(window, glm::uvec2());
  }
}

void render_pass_bloom_resize(app_state *state) {
  glm::uvec2 window = {state->window_area.w, state->window_area.h};
  for (int i = 0; i < MAX_BLOOM_ITERATIONS; i++) {
    texture_data t;
    t.w = window.x;
    t.h = window.y;
    t.data = nullptr;
    t.format = TEX_FORMAT_RGBA_F16;
    t.upscale = TEX_FILTER_LINEAR;
    t.downscale = TEX_FILTER_LINEAR;
    t.edge = TEX_EDGE_CLAMP;

    state->api.renderer.update_texture(&state->game.renderer.bloom_buffer_textures[i], &t);
    state->game.renderer.bloom_viewports[i] = window;

    if (window.x < 10 || window.y < 10) {
      state->game.renderer.bloom_iters = i;
      break;
    }

    window /= 2;
    window = glm::max(window, glm::uvec2(1));
  }
}
