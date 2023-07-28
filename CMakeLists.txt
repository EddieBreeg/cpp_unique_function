cmake_minimum_required(VERSION 3.0)
project(unique_function VERSION 0.0.1)

add_library(unique_function INTERFACE)
target_include_directories(unique_function INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main main.cpp)
target_link_libraries(main PRIVATE unique_function)