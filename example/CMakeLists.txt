
cmake_minimum_required(VERSION 3.21)

project("cpp_ecs_example")

add_executable(ecs_example
  main.cpp
)

set_target_properties(ecs_example PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/ecs)
target_link_libraries(ecs_example PRIVATE ecs)
