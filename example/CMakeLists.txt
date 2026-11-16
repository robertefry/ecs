
cmake_minimum_required(VERSION 3.21)

project("cpp_ecs_example")

add_executable(ecs_example
  main.cpp
)

set_target_properties(ecs_example PROPERTIES
  CXX_STANDARD 26
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/ecs)
target_link_libraries(ecs_example PUBLIC ecs)

file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage(
  NAME SDL2
  GITHUB_REPOSITORY libsdl-org/SDL
  GIT_TAG release-3.4.20
  OPTIONS
    "SDL_SHARED ON"
    "SDL_STATIC OFF"
    "SDL_TEST OFF"
)

target_link_libraries(ecs_example PUBLIC SDL3::SDL3)
