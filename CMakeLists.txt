cmake_minimum_required(VERSION 3.0)
project(libstra VERSION 0.0.1)

option(USE_CPP_17 "Enable support for C++ 17" ON)

if(${USE_CPP_17})
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 14)
endif(${USE_CPP_17})


add_library(libstra INTERFACE)
target_include_directories(libstra INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

option(BUILD_TESTS "Builds the test program" ON)

if(${BUILD_TESTS})
include(CTest)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(unique_function_test tests/unique_function.cpp)
target_link_libraries(unique_function_test PRIVATE libstra)

add_executable(storage_test tests/storage.cpp)
target_compile_features(storage_test PRIVATE cxx_std_14)
target_link_libraries(storage_test PRIVATE libstra)

add_executable(expected_test tests/expected.cpp)
target_compile_features(expected_test PRIVATE cxx_std_17)
target_link_libraries(expected_test PRIVATE libstra)

add_test(NAME UniqueFuntion COMMAND unique_function_test)
add_test(NAME Storage COMMAND storage_test)
add_test(NAME Expected COMMAND expected_test)
    
endif(${BUILD_TESTS})

