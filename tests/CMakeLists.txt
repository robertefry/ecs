
cmake_minimum_required(VERSION 3.21)

CPMAddPackage(
  NAME "Catch2"
  GITHUB_REPOSITORY "catchorg/Catch2"
  GIT_TAG "v3.16.1"
)

CPMAddPackage("gh:catchorg/Catch2@3.16.1")

add_executable(ecs_tests
  test_ecs.cc
)
target_set_standards(ecs_tests)

target_link_libraries(ecs_tests PRIVATE
  ecs
  Catch2::Catch2WithMain
)

add_test(NAME ecs_tests COMMAND $<TARGET_FILE:ecs_tests>)
