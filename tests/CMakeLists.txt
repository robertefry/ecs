
cmake_minimum_required(VERSION 3.21)

include(${CMAKE_CURRENT_LIST_DIR}/../cpm.cmake)

CPMAddPackage(
  NAME "Catch2"
  GITHUB_REPOSITORY "catchorg/Catch2"
  GIT_TAG "v3.16.1"
)

CPMAddPackage("gh:catchorg/Catch2@3.16.1")

add_executable(ecs_tests
)

target_link_libraries(ecs_tests PRIVATE Catch2::Catch2WithMain)
