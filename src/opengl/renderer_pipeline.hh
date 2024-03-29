#pragma once

#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "glad/gl.h"
#include "opengl/renderer_state.hh"
#include "pyrolib/log/assert.hh"
#include "tracy/Tracy.hpp"


static inline renderer_pipeline create_pipeline(pipeline_data *data) {
  ZoneScopedN("Create Pipeline");
  GLuint vertex, fragment, program;

  // Create shaders & program
  {
    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    program = glCreateProgram();
  }

  // Compile shaders
  {
    ZoneScopedN("Compile Shaders");
    glShaderSource(vertex, 1, &data->vertex_shader, nullptr);
    glShaderSource(fragment, 1, &data->fragment_shader, nullptr);

    glCompileShader(vertex);

    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      PYRO_LOGE("Vertex shader compile failed %s", infoLog);
    }

    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
      PYRO_LOGE("Fragment shader compile failed %s", infoLog);
    }
  }

  // Link Program
  {
    ZoneScopedN("Link Shaders");
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    int success;
    char infoLog[512];
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);

      PYRO_LOGE("Shader Link failed %s", infoLog);
    }
  }

  renderer_pipeline out;
  out.index = program;

  // Get uniforms
  {
    out.uniform_indicies.lt_init(rstate->perm_data, data->uniform_count);
    for (int i = 0; i < data->uniform_count; i++) {
      out.uniform_indicies[i] = glGetUniformLocation(program, data->uniform_names[i]);
    }
  }

  // Shaders no longer needed, so delete them
  {
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }

  return out;
}

static inline void delete_pipeline(renderer_pipeline pipeline) {
  ZoneScopedN("Delete Pipeline");
  glDeleteProgram(pipeline.index);
}
