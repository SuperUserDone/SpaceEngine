#pragma once

#include "data/asset_types.hh"
#include <glm/glm.hpp>
#include <stdint.h>

struct game_state {
  struct {
    glm::vec2 pos;
    float zoom;
  } camera;

  glm::vec3 sun_color;
};
