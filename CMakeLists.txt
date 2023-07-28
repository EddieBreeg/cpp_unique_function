cmake_minimum_required(VERSION 3.0)
project(unique_function VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

option(BUILD_TESTS "Builds the test program" ON)

add_library(unique_function INTERFACE)
target_include_directories(unique_function INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(${BUILD_TESTS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(main main.cpp)
target_link_libraries(main PRIVATE unique_function)

include(CTest)
enable_testing()
add_test(NAME Tests COMMAND main)
    
endif(${BUILD_TESTS})

