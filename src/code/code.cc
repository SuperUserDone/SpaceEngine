#include "assetmanager/assetmanager.hh"
#include "assetmanager/loader.hh"
#include "data/asset_types.hh"
#include "input/input.hh"
#include "renderer/renderer.hh"
#include "renderer/text/render_text.hh"
#include "scene_animation.hh"
#include "scene_garden.hh"
#include "scene_forest.hh"
#include "scene_start.hh"
#include "sprite_render.hh"
#include <imgui.h>
#include <string.h>

void load_assets(app_state *state) {
  asset_loader_load_file_sync(state, "data/engine.sdef").get_no_err();
  asset_loader_load_file_sync(state, "data/game.sdef").get_no_err();
}

void init(app_state *state) {
  state->game.camera.pos = {0, 0};
  state->game.camera.zoom = 2.f;
  state->game.camera.shake = 0.f;
  state->game.camera.shake_falloff = 150.f;

  state->game.renderer.bloom_params = {1.f, 0.9f, 0.2f, 2.5f};
  state->game.renderer.bloom_size = 0.001f;
  state->game.b = render_batch_create(state);

  /*state->game.solar_system = state->permanent_arena.push<solar_system>();
  state->game.solar_system->star.color = {255.f / 255.f, 113.f / 255.f, 0.f / 255.f};
  state->game.solar_system->star.pos = {0.f, 0.f};
  state->game.solar_system->star.raduis = 10.f;*/

  state->game.renderer.clear_color = glm::vec3(0.05, 0.053, 0.06);

  state->game.font_size = 32;
  state->game.text = new char[256];

  render_init(state);

  reset_game(state->game.gameplay);
}

void render(app_state *state) {
  render_game(state);
  renderer_font font = asset_font_get_render(state, "default"_sid);
  render_font_reset(state, font);
  // render_text(state, font, state->game.font_size, {100, 100}, state->game.text);

  switch (state->game.gameplay.scene) {
  case scene_type::GARDEN:
    garden::scene_text_render(state);
    break;
  case scene_type::ZONE_ANIMATION:
    anim::scene_text_render(state);
    break;
  case scene_type::FOREST:
    forest::scene_text_render(state);
    break;
  case scene_type::START:
    start::scene_text_render(state);
    break;
  }

  render_font_finish(state, font);
  render_sprites(state);
}

void tick(app_state *state) {
  state->game.camera.shake -=
      (state->game.camera.shake_falloff / 1000.f) * state->game.camera.shake;
  state->game.camera.shake = std::max(0.f, state->game.camera.shake);

  switch (state->game.gameplay.scene) {
  case scene_type::GARDEN:
    garden::scene_tick(state);
    break;
  case scene_type::ZONE_ANIMATION:
    anim::scene_tick(state);
    break;
  case scene_type::FOREST:
    forest::scene_tick(state);
    break;
  case scene_type::START:
    start::scene_tick(state);
    break;
  }
}

void shutdown(app_state *state) {
  // TODO
}

void draw_debug_info(app_state *state) {

  ImGui::Begin("DebugSettings");

  ImGui::DragFloat("Zoom", &state->game.camera.zoom, 0.1f, 0.01f, 100.f);
  ImGui::DragFloat2("Pos", (float *)&state->game.camera.pos);

  // ImGui::ColorEdit3("SunColor", (float *)&state->game.solar_system->star.color);
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
    render_font_info info =
        render_font_get_info(state, asset_font_get_render(state, "default"_sid));
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

  static int intensity = 30;

  ImGui::InputInt("Intensity", &intensity);
  if (ImGui::Button("Shake")) {
    state->game.camera.shake = intensity;
  }
  ImGui::DragFloat("Shake", &state->game.camera.shake, 1.f, 0.f, 1000.f);
  ImGui::DragFloat("Falloff", &state->game.camera.shake_falloff, 1.f, 0.f, 1000.f);

  input::debug_ui(state);

  debug_state(state->game.gameplay);

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
