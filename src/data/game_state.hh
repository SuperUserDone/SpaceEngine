#pragma once

#include <glm/glm.hpp>
#include <stdint.h>

struct game_state {
  struct {
    glm::vec2 pos;
    float zoom;
  } camera;
};
