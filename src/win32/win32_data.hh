#pragma once

#include <SDL2/SDL.h>
#include <windows.h>

struct win32_state {
  SDL_Window *window;
  SDL_GLContext glcontext;

  HINSTANCE renderer;
};