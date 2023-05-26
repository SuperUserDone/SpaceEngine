#pragma once

#include "data/asset_types.hh"
#include "data/game_data.hh"
#include <glm/glm.hpp>
#include <stdint.h>

#define MAX_BLOOM_ITERATIONS 12

struct game_state {
  struct {
    glm::vec2 pos;
    float zoom;
  } camera;

  solar_system *solar_system;

  struct {
    renderer_framebuffer bloom_buffers[MAX_BLOOM_ITERATIONS];
    renderer_texture bloom_buffer_textures[MAX_BLOOM_ITERATIONS];

    size_t bloom_iters;

    glm::vec2 bloom_viewports[MAX_BLOOM_ITERATIONS];
    glm::vec4 bloom_params;
    float bloom_size;

    glm::mat4 camv;
    glm::mat4 camp;

    glm::vec3 clear_color;

  } renderer;
};
