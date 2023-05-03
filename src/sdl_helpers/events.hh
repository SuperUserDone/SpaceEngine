#pragma once

#include "data/app_state.hh"
#include "data/event.hh"
#include <SDL2/SDL.h>

static inline event convert_sdl_event(SDL_Event &ev, SDL_Window *w) {
  event e;
  e.type = EVENT_TYPE_NONE;

  switch (ev.type) {
  case SDL_QUIT:
    e.type = EVENT_TYPE_QUIT;
    break;
  case SDL_WINDOWEVENT:
    switch (ev.window.event) {
    case SDL_WINDOWEVENT_RESIZED:
      e.type = EVENT_TYPE_RESIZE;
      SDL_GL_GetDrawableSize(w, &e.data.resize.newx, &e.data.resize.newy);
      break;
    }
    break;
  }

  return e;
}
