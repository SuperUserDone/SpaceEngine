#pragma once

#include "input/keys.hh"
#include <SDL2/SDL_scancode.h>

static inline input::keyboard::key key_from_sdl(SDL_Scancode code) {
  // Should be a 1-1 mapping
  return static_cast<input::keyboard::key>(code); 
}