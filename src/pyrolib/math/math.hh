#pragma once

#include <glm/glm.hpp>

namespace pyro {

struct vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec2 uv;
};

namespace math {

struct quad {
  typedef vertex *itterator;

  quad(vertex v1, vertex v2, vertex v3, vertex v4) {
    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    v[3] = v4;
  }

  quad(glm::vec2 pos, glm::vec2 size, glm::vec2 uva, glm::vec2 uvb) {
    v[0] = {{pos.x, pos.y, 0.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}, uva};
    v[1] = {{pos.x + size.x, pos.y, 0.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}, {uvb.x, uva.y}};
    v[2] = {glm::vec3(pos + size, 0.f), {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}, uvb};
    v[3] = {{pos.x, pos.y + size.y, 0.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}, {uva.x, uvb.y}};
  }

  itterator begin() {
    return v;
  }

  itterator end() {
    return &v[4];
  }

  vertex v[4];
};

} // namespace math
} // namespace pyro