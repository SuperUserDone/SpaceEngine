#pragma once 

#include "common/memory_arena.hh"
#include "data/app_state.hh"
#include "glad/gl.h"
#include "data/asset_types.hh"

static inline renderer_pipeline create_pipeline(pipeline_data *data) {
  GLuint vertex, fragment, program;

  // Create shaders & program
  {
    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    program = glCreateProgram();
  }

  // Compile shaders
  {
    glShaderSource(vertex, 1, &data->vertex_shader, nullptr);
    glShaderSource(fragment, 1, &data->fragment_shader, nullptr);

    glCompileShader(vertex);
    glCompileShader(fragment);

    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      SPACE_ASSERT(false, "Vertex shader compile failed %s", infoLog);
    }

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
      SPACE_ASSERT(false, "Fragment shader compile failed %s", infoLog);
    }
  }

  // Link Program
  {
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    int success;
    char infoLog[512];
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      
      SPACE_ASSERT(false, "Shader Link failed %s", infoLog);
    }
  }


  // Shaders no longer needed, so delete them
  {
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }

  return renderer_pipeline{program};
}

static inline void delete_pipeline(renderer_pipeline pipeline){
  glDeleteProgram(pipeline.index);
}


