
cmake_minimum_required(VERSION 3.21)
include(${CMAKE_CURRENT_LIST_DIR}/../standards.cmake)

project("cpp_ecs_example")

add_executable(ecs_example
  main.cpp
)
target_set_standards(ecs_example)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/ecs)
target_link_libraries(ecs_example PRIVATE ecs)
