#include <SDL2/SDL.h>

#include "client/TracyProfiler.hpp"
#include "tracy/Tracy.hpp"
#include <SDL_timer.h>
#include <SDL_video.h>
#include <stdio.h>
#include <windows.h>

#include "assetmanager/assetmanager.hh"
#include "common/debug.hh"
#include "common/memory_arena.hh"
#include "data/app_state.hh"
#include "win32/win32_data.hh"

static char module_name[MAX_PATH];

static mem_arena init_scratch;

void win32_err(const char *err) {
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR!", err, nullptr);
  exit(-1);
}

void hotreload_renderer(app_state *state);
void hotreload_code(app_state *state);

void run_game_loop(app_state *state) {
  win32_state *ws = (win32_state *)state->platform_state;
  state->running = true;
  arena_clear(init_scratch);

  state->time.dt = 0;
  state->time.t = 0;

  uint64_t start = SDL_GetPerformanceCounter();
  uint64_t dt_start = start;
  uint64_t freq = SDL_GetPerformanceFrequency();

  while (state->running) {
    arena_clear(state->frame_arena);

    SDL_Event e;
    {
      ZoneScopedN("Process Events");
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
          state->running = false;
        }
        if (e.type == SDL_KEYUP) {
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
        }
      }
    }
    
    SDL_GL_GetDrawableSize(ws->window, &state->window_area.w, &state->window_area.h);
    state->api.renderer.set_viewport(state->window_area.w, state->window_area.h);
    state->api.renderer.clear(0, 0, 0, 1);

    state->api.game.update(state);

    uint64_t now = SDL_GetPerformanceCounter();

    state->time.dt = ((double)(now - dt_start)) / (double)freq;
    state->time.t = ((double)(now - start)) / (double)freq;

    dt_start = now;
    if (state->time.t > 1024) {
      state->time.t = 0;
      start = now;
    }

    TracyPlot("Delta Time", state->time.dt);
    TracyPlot("Time", state->time.t);

    SDL_GL_SwapWindow(ws->window);
    FrameMark;
  }

  asset_system_shutdown(state);
}

void init_sdl() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

void create_window(app_state *state) {
  win32_state *ws = (win32_state *)state->platform_state;
  ws->window = SDL_CreateWindow("Space Game",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                800,
                                600,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  ws->glcontext = SDL_GL_CreateContext(ws->window);
}

void copy_dll(char *new_name, const char *dll) {
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
  state->api.game.init(state);
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

    asset_system_init(&state);
    {
      ZoneScopedN("Init Renderer");
      SPACE_ASSERT(
          state.api.renderer.init(state.frame_arena, &state, (load_proc)SDL_GL_GetProcAddress),
          "Failed to load renderer!");
    }

    {
      ZoneScopedN("Init Usercode");
      state.api.game.init(&state);
    }
  }

  run_game_loop(&state);

  free_renderer(&state);
  SDL_Quit();

  return 0;
}
