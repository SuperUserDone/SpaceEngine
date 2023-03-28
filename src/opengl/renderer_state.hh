#pragma once

#include "data/renderer_api.hh"

#include <stdint.h>

struct renderer_state {
  uint32_t shaders[mesh_render_material_type::COUNT];

  struct {
    uint32_t color_pos;
  } opaque_shader_data;
};