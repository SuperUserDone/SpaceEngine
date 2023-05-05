#pragma once

#include "data/asset_types.hh"
#include <glm/glm.hpp>
#include <stdint.h>

#define MAX_BLOOM_ITERATIONS 9 

struct game_state {
  struct {
    glm::vec2 pos;
    float zoom;
  } camera;

  glm::vec3 sun_color;

  struct {
    renderer_framebuffer bloom_buffers[MAX_BLOOM_ITERATIONS];
    renderer_texture bloom_buffer_textures[MAX_BLOOM_ITERATIONS];

    uint8_t bloom_iters;

    glm::vec2 bloom_viewports[MAX_BLOOM_ITERATIONS];
    float bloom_size;
  } renderer;
};
