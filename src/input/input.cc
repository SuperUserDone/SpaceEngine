#include "input/input.hh"
#include "data/app_state.hh"
#include "imgui.h"
#include "input/keys.hh"
#include "input/mouse_button.hh"

namespace input {

void reset(app_state *state) {
  state->raw_input.mouse_keyboard.mouse_wheel_vel = 0;
}

void debug_ui(app_state *state) {
  if (ImGui::CollapsingHeader("InputState")) {
    ImGui::SeparatorText("Mouse and Keyboard");

    ImGui::Text("Mouse Pos %f %f",
                state->raw_input.mouse_keyboard.mouse_pos.x,
                state->raw_input.mouse_keyboard.mouse_pos.y);
    ImGui::Text("Wheel vel %d", state->raw_input.mouse_keyboard.mouse_wheel_vel);

    ImGui::Text("Pressed Keys");
    for (size_t i = 0; i < keyboard::COUNT; i++) {
      if (state->raw_input.mouse_keyboard.keys[i] > 0) {
        ImGui::Text("%s %llu", keyboard::key_names[i], i);
      }
    }

    for (size_t i = 0; i < mouse::COUNT; i++) {
      if (state->raw_input.mouse_keyboard.mouse[i] > 0) {
        ImGui::Text("%s %llu", mouse::mouse_button_names[i], i);
      }
    }
  }
}

} // namespace input