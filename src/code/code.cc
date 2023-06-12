#include "assetmanager/assetmanager.hh"
#include "assetmanager/loader.hh"
#include "data/asset_types.hh"
#include "renderer/renderer.hh"
#include "renderer/text/render_text.hh"
#include <imgui.h>
#include <string.h>

void load_assets(app_state *state) {
  asset_loader_load_file_sync(state, "data/engine.sdef").get_no_err();
  asset_loader_load_file_sync(state, "data/game.sdef").get_no_err();
}

void init(app_state *state) {
  state->game.camera.pos = {0, 0};
  state->game.camera.zoom = 0.1f;

  state->game.renderer.bloom_params = {1.f, 0.9f, 0.2f, 2.5f};
  state->game.renderer.bloom_size = 0.001f;

  state->game.solar_system = state->permanent_arena.push<solar_system>();
  state->game.solar_system->star.color = {255.f / 255.f, 113.f / 255.f, 0.f / 255.f};
  state->game.solar_system->star.pos = {0.f, 0.f};
  state->game.solar_system->star.raduis = 10.f;

  state->game.renderer.clear_color = glm::vec3(0.05, 0.053, 0.06);

  state->game.font_size = 32;
  state->game.text = new char[256];

  strcpy_s(state->game.text,
           256,
           "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890-_=+");

  render_init(state);
}

void render(app_state *state) {
  render_game(state);
  renderer_font font = asset_font_get_render(state, "default");
  render_font_reset(state, font);
  render_text(state, font, state->game.font_size, {100, 100}, state->game.text);
  render_font_finish(state, font);
}

void tick(app_state *state) {
  // TODO
}

void shutdown(app_state *state) {
  // TODO
}

void draw_debug_info(app_state *state) {

  ImGui::Begin("DebugSettings");

  ImGui::DragFloat("Zoom", &state->game.camera.zoom, 0.1f, 0.01f, 100.f);
  ImGui::DragFloat2("Pos", (float *)&state->game.camera.pos);

  ImGui::ColorEdit3("SunColor", (float *)&state->game.solar_system->star.color);
  ImGui::ColorEdit3("ClearColor", (float *)&state->game.renderer.clear_color);

  if (ImGui::CollapsingHeader("Bloom")) {
    ImGui::DragFloat4("Bloom Threshold",
                      (float *)&state->game.renderer.bloom_params,
                      0.01,
                      0.f,
                      10.f);
    ImGui::DragFloat("Bloom Size", &state->game.renderer.bloom_size, 0.001f, 0.f, 1.f);
  }

  if (ImGui::CollapsingHeader("Text")) {
    ImGui::InputText("Display text", state->game.text, 256);

    int font_size = state->game.font_size;
    ImGui::SliderInt("Font size", &font_size, 1, 100);
    state->game.font_size = font_size;
    render_font_info info = render_font_get_info(state, asset_font_get_render(state, "default"));
    ImGui::Image((void *)(size_t)info.texture.index, {256, 256});

    ImGui::Text("CacheEntries %llu, HTEntries %llu, atlas_size = {%u, %u}",
                info.cache_entries,
                info.ht_entries,
                info.atlas_size.x,
                info.atlas_size.y);
  }
  if (ImGui::CollapsingHeader("Assets")) {
    /*for (auto &i : state->assets.) {
      ImGui::Text("%s\n", i.key);
    }*/
  }

  ImGui::End();
}

void process_event(app_state *state, event &e) {
  switch (e.type) {
  case EVENT_TYPE_NONE:
    break;
  case EVENT_TYPE_QUIT:
    state->running = false;
    break;
  case EVENT_TYPE_RESIZE:
    render_resize(state, e.data.resize.newx, e.data.resize.newy);
    break;
  }
}

extern "C" {
ALWAYS_EXPORT void fetch_api(app_state *state) {
  state->api.game.init = init;
  state->api.game.render = render;
  state->api.game.tick = tick;
  state->api.game.shutdown = shutdown;
  state->api.game.event = process_event;

  state->api.game.draw_debug_info = draw_debug_info;
  state->api.game.load_assets = load_assets;
}
}
