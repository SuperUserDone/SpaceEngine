#include "common/file_utils.hh"
#include "data/app_state.hh"

#include "common/defs.hh"
#include "common/win32_export.hh"
#include "glad/gl.h"

#include "opengl/renderer_state.hh"

#include <windows.h>

static inline uint32_t compile_shader(mem_arena &arena, const char *name, app_state *state) {

  char vertex_path[MAX_PATH];
  char fragment_path[MAX_PATH];

  // Set paths
  {
    char shader_path[MAX_PATH];

    strcpy_s(shader_path, MAX_PATH, state->working_dir);
    strcat_s(shader_path, MAX_PATH, PATH_SEPERATOR "data" PATH_SEPERATOR "shaders" PATH_SEPERATOR);
    strcat_s(shader_path, MAX_PATH, name);

    strcpy_s(vertex_path, MAX_PATH, shader_path);
    strcpy_s(fragment_path, MAX_PATH, shader_path);

    strcat_s(vertex_path, MAX_PATH, ".vert.glsl");
    strcat_s(fragment_path, MAX_PATH, ".frag.glsl");
  }

  char *vertex_data = load_file(arena, vertex_path);
  char *fragment_data = load_file(arena, fragment_path);

  GLuint vertex, fragment, program;

  // Create shaders & program
  {
    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    program = glCreateProgram();
  }

  // Compile shaders
  {
    glShaderSource(vertex, 1, &vertex_data, nullptr);
    glShaderSource(fragment, 1, &fragment_data, nullptr);

    glCompileShader(vertex);
    glCompileShader(fragment);

    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      printf("%s", infoLog);
    }

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
      printf("%s", infoLog);
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
      printf("%s", infoLog);
    }
  }

  // Shaders no longer needed, so delete them
  {
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }

  return program;
}