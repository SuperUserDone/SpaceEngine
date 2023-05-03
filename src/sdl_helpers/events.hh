#pragma once

#include "data/app_state.hh"
#include "data/event.hh"
#include <SDL2/SDL_events.h>

static inline event convert_sdl_event(SDL_Event &ev) {
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
      e.data.resize.newx = ev.window.data1;
      e.data.resize.newy = ev.window.data2;
      break;
    }
    break;
  }

  return e;
}
