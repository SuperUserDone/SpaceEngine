#include "assetmanager/assetmanager.hh"
#include "backends/imgui_impl_sdl2.h"
#include "common/debug.hh"
#include "common/timer.hh"
#include "data/app_state.hh"
#include "data/event.hh"
#include "imgui.h"
#include "memory/memory_arena.hh"
#include "sdl_helpers/events.hh"
#include "tracy/Tracy.hpp"
#include "win32/win32_data.hh"

#include <SDL_hints.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <stdio.h>
#include <windows.h>

static char module_name[MAX_PATH];

static mem_arena init_scratch;

void win32_err(const char *err, bool die) {
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR!", err, nullptr);
  if (die)
    exit(-1);
}

void hotreload_renderer(app_state *state);
void hotreload_code(app_state *state);

void init_imgui(app_state *state) {
  ZoneScopedN("Init Imgui");

  win32_state *ws = (win32_state *)state->platform_state;
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(ws->window, ws->glcontext);
}

void process_sdl_event_internal(SDL_Event &e, app_state *state, bool &debug_ui) {
  if (e.key.keysym.scancode == SDL_SCANCODE_F5) {
    // check if buildscript exists
    if (!(INVALID_FILE_ATTRIBUTES == GetFileAttributes("build-dlls.bat") &&
          GetLastError() == ERROR_FILE_NOT_FOUND)) {
      PROCESS_INFORMATION info;
      STARTUPINFO start_info;

      ZeroMemory(&start_info, sizeof(STARTUPINFO));
      start_info.cb = sizeof(start_info);
      ZeroMemory(&info, sizeof(PROCESS_INFORMATION));
      // TODO Display the output of this on the window using pipes

      // Run the build script
      char args[] = "/c build-dlls.bat";
      CreateProcess(TEXT("C:\\Windows\\System32\\cmd.exe"),
                    TEXT(args),
                    nullptr,
                    nullptr,
                    false,
                    NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
                    nullptr,
                    nullptr,
                    &start_info,
                    &info);

      // Wait until child process exits.
      WaitForSingleObject(info.hProcess, INFINITE);

      // Close process and thread handles.
      CloseHandle(info.hProcess);
      CloseHandle(info.hThread);
    }

    hotreload_renderer(state);
    hotreload_code(state);
  }
  if (e.key.keysym.scancode == SDL_SCANCODE_F1)
    debug_ui = !debug_ui;
}

void run_game_loop(app_state *state) {
  win32_state *ws = (win32_state *)state->platform_state;

  state->running = true;
  arena_clear(init_scratch);

  state->time.dt = 0;
  state->time.t = 0;

  timer_init_subsystem();
  precise_timer delta_timer = timer_create();
  precise_timer time_timer = timer_create();
  precise_timer tick_timer = timer_create();

  bool debug_ui = false;
  state->tps_target = 60;

  while (state->running) {
    ZoneScopedN("Render");

    // Setup
    {
      ZoneScopedN("Setup");
      arena_clear(state->frame_arena);
    }

    // Process Events
    {
      ZoneScopedN("Process Events");
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);

        event translated = convert_sdl_event(e, ws->window);

        if (translated.type == EVENT_TYPE_RESIZE) {
          state->window_area.w = translated.data.resize.newx;
          state->window_area.h = translated.data.resize.newy;
        }

        if (translated.type != EVENT_TYPE_NONE)
          state->api.game.event(state, translated);

        if (e.type == SDL_KEYUP) {
          process_sdl_event_internal(e, state, debug_ui);
        }
      }
    }

    // Window stuff
    {
      int winx, winy;
      SDL_GetWindowSize(ws->window, &winx, &winy);
      SDL_GL_GetDrawableSize(ws->window, &state->window_area.w, &state->window_area.h);
      state->window_area.dpi_scaling = (float)state->window_area.w / (float)winx;
    }

    // Delta time stuff
    {
      state->time.dt = (double)timer_reset_us(delta_timer) / 1000000.0;
      state->time.t = (double)timer_get_time_us(time_timer) / 1000000.0;
      if (state->time.t >= 1024) {
        timer_reset_us(time_timer);
      }

      TracyPlot("Delta Time", state->time.dt);
      TracyPlot("Time", state->time.t);
    }

    // Run game update
    {
      ZoneScopedN("ProcessTicks");
      int64_t tick = timer_get_time_us(tick_timer);
      int64_t time_per_tick = 1000000 / state->tps_target;

      while (tick > time_per_tick) {
        tick -= time_per_tick;
        timer_reset_us(tick_timer);
        FrameMarkNamed("Tick");
        state->api.game.tick(state);
      }
    }

    // Render Screen
    {
      ZoneScopedN("Render");
      state->api.game.render(state);
    }

    // Render Debug UI
    {
      ZoneScopedN("RenderDebugUI");
      if (debug_ui) {
        ImGui_ImplSDL2_NewFrame();
        state->api.renderer.imgui_begin();

        state->api.game.draw_debug_info(state);

        state->api.renderer.imgui_end();
      }
    }

    // Present screen
    {
      ZoneScopedN("Present");
      SDL_GL_SwapWindow(ws->window);
      FrameMark;
    }
  }

  asset_system_shutdown(state);
}

void init_sdl() {
  ZoneScopedN("Init SDL");
  // Set dpi aware on windows
  SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");
  SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "1");
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

void create_window(app_state *state) {
  ZoneScopedN("Create Window");
  win32_state *ws = (win32_state *)state->platform_state;
  ws->window =
      SDL_CreateWindow("Space Game",
                       SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED,
                       800,
                       600,
                       SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  ws->glcontext = SDL_GL_CreateContext(ws->window);
  SDL_GL_MakeCurrent(ws->window, ws->glcontext);
  SDL_GL_GetDrawableSize(ws->window, &state->window_area.w, &state->window_area.h);
}

void copy_dll(char *new_name, const char *dll) {
  ZoneScopedN("Copy DLL");
  char name[MAX_PATH];

  strcpy_s(name, MAX_PATH, module_name);
  strcat_s(name, MAX_PATH, dll);
  strcat_s(name, MAX_PATH, ".dll");

#ifndef NDEBUG
  strcpy_s(new_name, MAX_PATH, module_name);
  strcat_s(new_name, MAX_PATH, dll);
  strcat_s(new_name, MAX_PATH, "-hot-reload.dll");

  DeleteFile(new_name);
  CopyFile(name, new_name, false);
#else
  strcpy_s(new_name, MAX_PATH, name);
#endif
}
void load_code(app_state *state) {
  win32_state *ws = (win32_state *)state->platform_state;

  char new_name[MAX_PATH];

  copy_dll(new_name, "space_code");

  ws->code = nullptr;
  ws->code = LoadLibrary(TEXT(new_name));

  ((fetch_api_fun)GetProcAddress(ws->code, "fetch_api"))(state);
}

void free_code(app_state *state) {
  win32_state *ws = (win32_state *)state->platform_state;
  FreeLibrary(ws->code);
}

void hotreload_code(app_state *state) {
  free_code(state);
  load_code(state);
  state->api.game.load_assets(state);
}

void load_renderer(app_state *state) {
  win32_state *ws = (win32_state *)state->platform_state;

  char new_name[MAX_PATH];

  copy_dll(new_name, "space_opengl");

  ws->renderer = LoadLibrary(TEXT(new_name));

  ((fetch_api_fun)GetProcAddress(ws->renderer, "fetch_api"))(state);
}

void free_renderer(app_state *state) {
  win32_state *ws = (win32_state *)state->platform_state;
  FreeLibrary(ws->renderer);
}

void hotreload_renderer(app_state *state) {
  state->api.renderer.shutdown();

  free_renderer(state);
  load_renderer(state);

  state->api.renderer.init(init_scratch, state, (load_proc)SDL_GL_GetProcAddress);
  arena_clear(init_scratch);
}

int main(int argc, char *argv[]) {
  app_state state;

  {
    ZoneScopedN("Init");
    platform_err = win32_err;

    state.permanent_arena = arena_create();
    state.platform_state = arena_push_struct(state.permanent_arena, win32_state);
    state.frame_arena = arena_create();
    state.renderer_state = nullptr;

    GetModuleFileName(nullptr, module_name, MAX_PATH);
    size_t len = strlen(module_name);

    for (int i = len; i > 0; i--) {
      if (module_name[i] == '\\' || module_name[i] == '/') {
        module_name[i + 1] = 0;
        break;
      }
    }

    GetCurrentDirectory(260, state.working_dir);

    init_sdl();
    create_window(&state);
    load_renderer(&state);
    load_code(&state);

    init_imgui(&state);

    {
      ZoneScopedN("Init Renderer");
      SPACE_ASSERT(
          state.api.renderer.init(state.frame_arena, &state, (load_proc)SDL_GL_GetProcAddress),
          "Failed to load renderer!");

      asset_system_init(&state);
    }

    {
      ZoneScopedN("Init Usercode");
      state.api.game.load_assets(&state);
      state.api.game.init(&state);
    }
  }

  run_game_loop(&state);

  free_renderer(&state);
  SDL_Quit();

  return 0;
}
