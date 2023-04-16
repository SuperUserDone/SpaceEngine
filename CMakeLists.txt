cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake")

project(SpaceGame)

option(ENABLE_PROFILING "" OFF)

find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src)

add_library(space_common SHARED src/common/memory_arena.cc src/common/debug.cc src/assetmanager/assetmanager.cc)
add_library(space_opengl SHARED src/opengl/renderer.cc)
add_library(space_code SHARED src/code/code.cc)
add_executable(space WIN32 src/win32/win32_space.cc)

include_directories(tracy/public)
if(${ENABLE_PROFILING})
  add_library(tracy SHARED tracy/public/TracyClient.cpp)
  target_compile_definitions(tracy PRIVATE TRACY_EXPORTS TRACY_ENABLE)
  set(ADD_DEFINES TRACY_IMPORTS TRACY_ENABLE)
  set(ADD_LINKS tracy)
endif()

target_link_libraries(space_common glm::glm ${ADD_LINKS})

target_compile_definitions(space_common PRIVATE EXPORT_WIN PUBLIC ${ADD_DEFINES})

target_link_libraries(space
    PRIVATE
    space_common
    glm::glm
    ${ADD_LINKS}
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

target_link_libraries(space_code PRIVATE space_common glm::glm ${ADD_LINKS})
target_link_libraries(space_opengl PRIVATE space_common glm::glm ${ADD_LINKS})

add_dependencies(space space_opengl space_common space_code)

set_target_properties(space_opengl PROPERTIES PDB_NAME "space_opengl_%random%")
set_target_properties(space_code PROPERTIES PDB_NAME "space_code_%random%")
