cmake_minimum_required(VERSION 3.0)
project(libstra VERSION 0.1.0)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


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
target_link_libraries(storage_test PRIVATE libstra)


add_executable(utils_tests tests/utils.cpp)
target_link_libraries(utils_tests PRIVATE libstra)

add_test(NAME UniqueFuntion COMMAND unique_function_test)
add_test(NAME Storage COMMAND storage_test)
add_test(NAME Utility COMMAND utils_tests)

    
endif(${BUILD_TESTS})

option(INSTALL "Activates library installation" ON)

if(${INSTALL})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/libstra DESTINATION include)
    install(FILES LibstraConfig.cmake DESTINATION cmake)
endif(${INSTALL})
