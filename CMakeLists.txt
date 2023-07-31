cmake_minimum_required(VERSION 3.0)
project(libstra VERSION 0.1.0)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${MSVC})
    add_compile_options(/Wall)
    else()    
    add_compile_options(-Wall)
endif(${MSVC})

file(GLOB LIBSTRA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
add_library(libstra STATIC ${LIBSTRA_SRC})
target_include_directories(libstra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

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

add_executable(thread_pool_tests tests/thread_pool.cpp)
target_link_libraries(thread_pool_tests PRIVATE libstra)

add_executable(latch_tests tests/latch.cpp)
target_link_libraries(latch_tests PRIVATE libstra)

add_test(NAME UniqueFuntion COMMAND unique_function_test)
add_test(NAME Storage COMMAND storage_test)
add_test(NAME Utility COMMAND utils_tests)
add_test(NAME ThreadPool COMMAND thread_pool_tests)
add_test(NAME Latch COMMAND latch_tests)

    
endif(${BUILD_TESTS})

option(INSTALL "Activates library installation" ON)

if(${INSTALL})
    install(TARGETS libstra DESTINATION lib)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/libstra DESTINATION include)
    install(FILES LibstraConfig.cmake DESTINATION lib/cmake)
endif(${INSTALL})
