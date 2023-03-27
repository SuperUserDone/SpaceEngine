#include <SDL2/SDL.h>

#include <stdio.h>
#include <windows.h>

#include "common/memory_arena.hh"
#include "data/app_state.hh"
#include "win32/win32_data.hh"

static char module_name[MAX_PATH];

void hotreload_renderer(app_state *state);

void run_game_loop(mem_arena &arena, app_state *state) {
  mem_arena frame_arena = arena_create();

  win32_state *ws = (win32_state *)state->platform_state;
  state->running = true;

  if (!state->api.renderer.init((load_proc)SDL_GL_GetProcAddress)) {
    // TODO Error
  } else {
    while (state->running) {
      arena_clear(frame_arena);

      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
          state->running = false;
        }
        if (e.type == SDL_KEYUP) {
          if (e.key.keysym.scancode == SDL_SCANCODE_F5)
            hotreload_renderer(state);
        }
      }

      int sizex, sizey;
      SDL_GetWindowSizeInPixels(ws->window, &sizex, &sizey);
      state->api.renderer.clear(1, 1, 1, 1);

      SDL_GL_SwapWindow(ws->window);
    }
  }

  arena_free(frame_arena);
}

void init_sdl() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

void create_window(mem_arena &arena, app_state *state) {
  win32_state *ws = (win32_state *)state->platform_state;
  ws->window = SDL_CreateWindow(
      "Space Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
  ws->glcontext = SDL_GL_CreateContext(ws->window);
}

void copy_dll(char *new_name, const char *dll) {
  char name[MAX_PATH];

  strcpy_s(name, MAX_PATH, module_name);
  strcat_s(name, MAX_PATH, dll);
  strcat_s(name, MAX_PATH, ".dll");

  strcpy_s(new_name, MAX_PATH, module_name);
  strcat_s(new_name, MAX_PATH, dll);
  strcat_s(new_name, MAX_PATH, "-hot-reload");
  strcat_s(new_name, MAX_PATH, ".dll");

  CopyFile(name, new_name, false);
}

void load_renderer(app_state *state) {
  win32_state *ws = (win32_state *)state->platform_state;

  char new_name[MAX_PATH];

  copy_dll(new_name, "space_opengl");

  ws->renderer = LoadLibrary(TEXT(new_name));

  state->api.renderer = ((renderer_fetch_api_fun)GetProcAddress(ws->renderer, "fetch_api"))();
}

void free_renderer(app_state *state) {
  win32_state *ws = (win32_state *)state->platform_state;
  FreeLibrary(ws->renderer);
}

void hotreload_renderer(app_state *state) {
  state->api.renderer.shutdown();

  free_renderer(state);
  load_renderer(state);

  state->api.renderer.init((load_proc)SDL_GL_GetProcAddress);
}

int main(int argc, char *argv[]) {
  mem_arena main_arena = arena_create();

  app_state *state = PushStruct(main_arena, app_state);
  state->platform_state = PushStruct(main_arena, win32_state);

  GetModuleFileName(nullptr, module_name, MAX_PATH);
  size_t len = strlen(module_name);

  for (int i = len; i > 0; i--) {
    if (module_name[i] == '\\' || module_name[i] == '/') {
      module_name[i + 1] = 0;
      break;
    }
  }

  init_sdl();
  create_window(main_arena, state);
  load_renderer(state);

  run_game_loop(main_arena, state);

  free_renderer(state);
  arena_free(main_arena);
  SDL_Quit();

  return 0;
}