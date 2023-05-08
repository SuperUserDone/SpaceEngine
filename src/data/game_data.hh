#pragma once 

#include <glm/glm.hpp>

struct star {
  glm::vec3 color;
  glm::vec2 pos;
  float raduis;
};

struct solar_system {
  star star;
};
