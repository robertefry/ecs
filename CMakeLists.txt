
cmake_minimum_required(VERSION 3.21)
include(cpm.cmake)
include(standards.cmake)

project("cpp_ecs" VERSION 0.1.0)

add_library(ecs INTERFACE
)
target_set_standards(ecs)

target_include_directories(ecs
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

option(ECS_TESTS "Enable ECS tests" ON)
if (ECS_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
