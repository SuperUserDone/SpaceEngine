#pragma once

#include "renderer/render_batch.hh"
#include <array>
#include <imgui.h>
#include <glm/glm.hpp>

enum class scene_type {
  GARDEN,
  ZONE_ANIMATION,
  FOREST,
  START,
};

enum class season_type {
  SUMMER,
  AUTUMN,
  WINTER,
  SPRING
};

static inline const char *season_names[] = {"Summer", "Autumn", "Winter", "Spring"};
static inline const char *get_seson_name(season_type type) {
  return season_names[static_cast<size_t>(type)];
}

struct game_data {
  int seeds;
  int health;

  float speed;

  struct {
    struct {
      bool planted;
      bool unfertile;
    } plots[3];
  } garden;

  scene_type scene;
  season_type season;

  float anim_len;
  float anim_progress;

  glm::vec2 player_pos;
};

static inline void reset_game(game_data &data) {
  data.seeds = 0;
  data.health = 3;
  data.speed = 1.f;
  data.anim_len = 3.f;
  data.anim_progress = 0.f;
  data.player_pos = {0.f, 0.f};

  for (size_t i = 0; i < sizeof(data.garden.plots) / sizeof(data.garden.plots[0]); i++) {
    data.garden.plots[i] = {false, false};
  }

  data.scene = scene_type::START;
  data.season = season_type::SUMMER;
}

static inline void debug_state(game_data &data) {
  if (ImGui::CollapsingHeader("Game state")) {
    ImGui::InputInt("Seeds", &data.seeds);
    ImGui::InputInt("Health", &data.health);
    ImGui::InputFloat("Speed", &data.speed);

    if (ImGui::BeginCombo("Season", get_seson_name(data.season))) {
      constexpr std::array<season_type, 4> seasons = {season_type::SUMMER,
                                                      season_type::AUTUMN,
                                                      season_type::SPRING,
                                                      season_type::WINTER};
      for (auto &a : seasons) {
        if (ImGui::Selectable(get_seson_name(a))) {
          data.season = a;
        }
      }
      ImGui::EndCombo();
    }
  }
}

static inline void switch_season(game_data &data, season_type type) {
  data.season = type;

  for (size_t i = 0; i < sizeof(data.garden.plots) / sizeof(data.garden.plots[0]); i++) {
    data.garden.plots[i].unfertile = data.garden.plots[i].planted;
    data.garden.plots[i].planted = false;
  }

  data.scene = scene_type::ZONE_ANIMATION;
  data.anim_progress = 0.f;
}
