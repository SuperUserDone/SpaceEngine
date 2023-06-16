#pragma once

#include "data/app_state.hh"
#include "input/keys.hh"
#include "input/mouse_button.hh"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>

static inline void sdl_read_input(app_state *state) {
  SDL_PumpEvents();
  // Keyboard
  const uint8_t *keys = SDL_GetKeyboardState(nullptr);
  // Should be optimized to a memset or a SIMD copy we dont realy care as we trust the compiler
  // should do a proper job.
  for (size_t i = 0; i < input::keyboard::key::COUNT; i++) {
    state->raw_input.mouse_keyboard.keys[i] = keys[i];
  }

  // Mouse
  int x, y;
  uint32_t mouse_state = SDL_GetMouseState(&x, &y);
  state->raw_input.mouse_keyboard.mouse[input::mouse::button::LEFT] =
      mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT);
  state->raw_input.mouse_keyboard.mouse[input::mouse::button::RIGHT] =
      mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT);
  state->raw_input.mouse_keyboard.mouse[input::mouse::button::MIDDLE] =
      mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE);
  state->raw_input.mouse_keyboard.mouse[input::mouse::button::EXTRA_1] =
      mouse_state & SDL_BUTTON(4);
  state->raw_input.mouse_keyboard.mouse[input::mouse::button::EXTRA_2] =
      mouse_state & SDL_BUTTON(5);
  state->raw_input.mouse_keyboard.mouse[input::mouse::button::EXTRA_3] =
      mouse_state & SDL_BUTTON(6);
  state->raw_input.mouse_keyboard.mouse[input::mouse::button::EXTRA_4] =
      mouse_state & SDL_BUTTON(7);
  state->raw_input.mouse_keyboard.mouse[input::mouse::button::EXTRA_5] =
      mouse_state & SDL_BUTTON(8);

  state->raw_input.mouse_keyboard.mouse_pos = {x, y};
}

static inline void sdl_process_input_event(app_state *state, SDL_Event &e) {
  if (e.type == SDL_MOUSEWHEEL) {
    state->raw_input.mouse_keyboard.mouse_wheel_vel = e.wheel.y;
  }
}