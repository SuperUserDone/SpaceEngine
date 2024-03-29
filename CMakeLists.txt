cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake")

project(SpaceGame)

option(ENABLE_PROFILING "" OFF)

find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(Freetype REQUIRED)

set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src imgui extern)
include_directories(src/pyrolib/extern)

add_library(imgui SHARED
  imgui/backends/imgui_impl_opengl3.cpp
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  )

add_library(space_common SHARED 
  src/assetmanager/asset_set.cc
  src/assetmanager/assetmanager.cc
  src/assetmanager/loader.cc
  src/common/sdef_parser.cc
  src/input/input.cc
  src/renderer/render_batch.cc
  src/renderer/render_pass_bloom.cc
  src/renderer/render_pass_geometry.cc
  src/renderer/render_pass_tonemap.cc
  src/renderer/render_utils.cc
  src/renderer/renderer.cc
  src/renderer/text/glyph_builder.cc
  src/renderer/text/render_text.cc
  )

add_library(space_opengl SHARED src/opengl/renderer.cc)
add_library(space_code SHARED src/code/code.cc)
add_executable(space WIN32 src/win32/win32_space.cc imgui/backends/imgui_impl_sdl2.cpp)

include_directories(tracy/public)
if(${ENABLE_PROFILING})
  add_library(tracy SHARED tracy/public/TracyClient.cpp)
  set(TRACY_DEFINES TRACY_ENABLE TRACY_NO_CALLSTACK_INLINES)
  target_compile_definitions(tracy PRIVATE TRACY_EXPORTS ${TRACY_DEFINES})
  set(ADD_DEFINES TRACY_IMPORTS ${TRACY_DEFINES})
  add_compile_definitions(${ADD_DEFINES})
  set(ADD_LINKS tracy)
endif()

add_subdirectory(src/pyrolib)
target_link_libraries(space_common pyro glm::glm imgui Freetype::Freetype harfbuzz::harfbuzz ${ADD_LINKS})

target_compile_definitions(space_common PRIVATE EXPORT_WIN)
target_compile_definitions(imgui PRIVATE "IMGUI_API=__declspec(dllexport)")

target_link_libraries(space
    PRIVATE
    space_common
    glm::glm
    imgui
    ${ADD_LINKS}
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

target_link_libraries(space_code PRIVATE space_common glm::glm ${ADD_LINKS})
target_link_libraries(space_opengl PRIVATE space_common glm::glm ${ADD_LINKS})

add_dependencies(space space_opengl space_common space_code)

set_target_properties(space_opengl PROPERTIES PDB_NAME "space_opengl_%random%")
set_target_properties(space_code PROPERTIES PDB_NAME "space_code_%random%")
