#pragma once

// Simple utils to help the user to assign uniforms a bit safer than setting them with arrays. It
// does simple type checking too

#include "pyrolib/log/assert.hh"
#include "data/asset_types.hh"
#include <glm/glm.hpp>
#include <pyrolib/container/array.hh>
#include <pyrolib/memory/arena.hh>
#include <stdint.h>

enum uniform_type {
  UNIFORM_TYPE_SCALAR,
  UNIFORM_TYPE_INTEGER,
  UNIFORM_TYPE_TEXTURE,
  UNIFORM_TYPE_VEC2,
  UNIFORM_TYPE_VEC3,
  UNIFORM_TYPE_VEC4,
  UNIFORM_TYPE_MAT4,
};

enum blending_func {
  BLEND_ADD,
  BLEND_ONE_MIN_SRC_ALPHA,
};

struct renderer_uniform {
  uniform_type type;
  uint32_t index;

  union {
    renderer_texture texture;
    float scalar;
    int32_t integer;
    glm::vec2 vec2;
    glm::vec3 vec3;
    glm::vec4 vec4;
    glm::mat4 mat4;
  };
};

struct pipeline_settings {
  pyro::container::array<renderer_uniform> uniforms;
  pyro::container::array<bool> index_assigned;
  size_t current_pos;
};

static inline pipeline_settings pipeline_settings_create(renderer_pipeline p,
                                                         pyro::memory::arena &arena) {
  pipeline_settings s;

  s.uniforms.lt_init(arena, p.uniform_indicies.size());
  s.index_assigned.lt_init(arena, p.uniform_indicies.size());
  s.current_pos = 0;

  return s;
};

static inline void pipeline_settings_set_uniform(pipeline_settings &s, size_t index, float val) {
  PYRO_ASSERT(
      s.current_pos < s.uniforms.size(),
      "Trying to assign more uniforms than exists to pipeline settings! Setting %llu to %f!",
      index,
      val);
  PYRO_ASSERT(!s.index_assigned[s.current_pos],
               "Assigning to uniform varible that was already assigned to!");

  s.index_assigned[s.current_pos] = true;

  s.uniforms[s.current_pos].index = index;
  s.uniforms[s.current_pos].type = UNIFORM_TYPE_SCALAR;
  s.uniforms[s.current_pos++].scalar = val;
}

static inline void pipeline_settings_set_uniform(pipeline_settings &s, size_t index, int32_t val) {
  PYRO_ASSERT(s.current_pos < s.uniforms.size(),
               "Trying to assign more uniforms than exists to pipeline settings! Setting integer "
               "%llu to %d!",
               index,
               val);
  PYRO_ASSERT(!s.index_assigned[s.current_pos],
               "Assigning to uniform varible that was already assigned to!");

  s.index_assigned[s.current_pos] = true;

  s.uniforms[s.current_pos].index = index;
  s.uniforms[s.current_pos].type = UNIFORM_TYPE_INTEGER;
  s.uniforms[s.current_pos++].integer = val;
}

static inline void pipeline_settings_set_uniform(pipeline_settings &s,
                                                 size_t index,
                                                 renderer_texture val) {
  PYRO_ASSERT(s.current_pos < s.uniforms.size(),
               "Trying to assign more uniforms than exists to pipeline settings! Setting texture "
               "%llu to %d!",
               index,
               val.index);
  PYRO_ASSERT(!s.index_assigned[s.current_pos],
               "Assigning to uniform varible that was already assigned to!");

  s.index_assigned[s.current_pos] = true;

  s.uniforms[s.current_pos].index = index;
  s.uniforms[s.current_pos].type = UNIFORM_TYPE_TEXTURE;
  s.uniforms[s.current_pos++].texture = val;
}

static inline void pipeline_settings_set_uniform(pipeline_settings &s,
                                                 size_t index,
                                                 glm::vec2 val) {
  PYRO_ASSERT(s.current_pos < s.uniforms.size(),
               "Trying to assign more uniforms than exists to pipeline settings! Setting vec2 %llu "
               "to {%f, %f}!",
               index,
               val.x,
               val.y);
  PYRO_ASSERT(!s.index_assigned[s.current_pos],
               "Assigning to uniform varible that was already assigned to!");

  s.index_assigned[s.current_pos] = true;

  s.uniforms[s.current_pos].index = index;
  s.uniforms[s.current_pos].type = UNIFORM_TYPE_VEC2;
  s.uniforms[s.current_pos++].vec2 = val;
}

static inline void pipeline_settings_set_uniform(pipeline_settings &s,
                                                 size_t index,
                                                 glm::vec3 val) {
  PYRO_ASSERT(s.current_pos < s.uniforms.size(),
               "Trying to assign more uniforms than exists to pipeline settings! Setting vec3 %llu "
               "to {%f, %f, %f}!",
               index,
               val.x,
               val.y,
               val.z);
  PYRO_ASSERT(!s.index_assigned[s.current_pos],
               "Assigning to uniform varible that was already assigned to!");

  s.index_assigned[s.current_pos] = true;

  s.uniforms[s.current_pos].index = index;
  s.uniforms[s.current_pos].type = UNIFORM_TYPE_VEC3;
  s.uniforms[s.current_pos++].vec3 = val;
}

static inline void pipeline_settings_set_uniform(pipeline_settings &s,
                                                 size_t index,
                                                 glm::vec4 val) {
  PYRO_ASSERT(s.current_pos < s.uniforms.size(),
               "Trying to assign more uniforms than exists to pipeline settings! Setting %llu to "
               "{%f, %f, %f, %f}!",
               index,
               val.x,
               val.y,
               val.z,
               val.w);
  PYRO_ASSERT(!s.index_assigned[s.current_pos],
               "Assigning to uniform varible that was already assigned to!");

  s.index_assigned[s.current_pos] = true;

  s.uniforms[s.current_pos].index = index;
  s.uniforms[s.current_pos].type = UNIFORM_TYPE_VEC4;
  s.uniforms[s.current_pos++].vec4 = val;
}

static inline void pipeline_settings_set_uniform(pipeline_settings &s,
                                                 size_t index,
                                                 glm::mat4 val) {
  PYRO_ASSERT(
      s.current_pos < s.uniforms.size(),
      "Trying to assign more uniforms than exists to pipeline settings! Setting %llu to matrix!",
      index);
  PYRO_ASSERT(!s.index_assigned[s.current_pos],
               "Assigning to uniform varible that was already assigned to!");

  s.index_assigned[s.current_pos] = true;

  s.uniforms[s.current_pos].index = index;
  s.uniforms[s.current_pos].type = UNIFORM_TYPE_MAT4;
  s.uniforms[s.current_pos++].mat4 = val;
}
