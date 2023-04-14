cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake")

project(SpaceGame)

find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(xxHash CONFIG REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src)

add_library(space_common SHARED src/common/memory_arena.cc src/common/debug.cc src/assetmanager/assetmanager.cc)
add_library(space_opengl SHARED src/opengl/renderer.cc)
add_library(space_code SHARED src/code/code.cc)
add_executable(space WIN32 src/win32/win32_space.cc)

target_compile_definitions(space_common PRIVATE EXPORT_WIN)

target_link_libraries(space_common PUBLIC xxHash::xxhash)

target_link_libraries(space
    PRIVATE
    space_common
    glm::glm
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

target_link_libraries(space_code PRIVATE space_common glm::glm)
target_link_libraries(space_opengl PRIVATE space_common glm::glm)

add_dependencies(space space_opengl space_common space_code)

set_target_properties(space_opengl PROPERTIES PDB_NAME "space_opengl_%random%")
set_target_properties(space_code PROPERTIES PDB_NAME "space_code_%random%")
