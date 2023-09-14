cmake_minimum_required(VERSION 3.2...3.5)
project(libstra VERSION 1.0.0)


if(NOT ${MSVC})
    add_compile_options(-Wall)
endif(${MSVC})

file(GLOB LIBSTRA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(CPP_STANDARD 14 CACHE STRING "The C++ standard to use")
set_property(CACHE CPP_STANDARD PROPERTY STRINGS 14 17 20 23)

set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
add_library(libstra STATIC ${LIBSTRA_SRC})



target_compile_features(libstra PUBLIC cxx_std_14)
target_include_directories(libstra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


include(CTest)

if(${BUILD_TESTING})
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

add_executable(sem_tests tests/semaphore.cpp)
target_link_libraries(sem_tests PRIVATE libstra)

add_executable(barrier_tests tests/barrier.cpp)
target_link_libraries(barrier_tests PRIVATE libstra)

add_executable(memory_tests tests/memory.cpp)
target_link_libraries(memory_tests PRIVATE libstra)

add_executable(views_tests tests/views.cpp)
target_link_libraries(views_tests PRIVATE libstra)

add_executable(static_vector_tests tests/static_vector.cpp)
target_link_libraries(static_vector_tests PRIVATE libstra)

add_executable(range_tests tests/ranges.cpp)
target_link_libraries(range_tests PRIVATE libstra)
add_executable(iterator_tests tests/iterators.cpp)
target_link_libraries(iterator_tests PRIVATE libstra)

add_test(NAME UniqueFuntion COMMAND unique_function_test)
add_test(NAME Storage COMMAND storage_test)
add_test(NAME Utility COMMAND utils_tests)
add_test(NAME ThreadPool COMMAND thread_pool_tests)
add_test(NAME Latch COMMAND latch_tests)
add_test(NAME Semaphore COMMAND sem_tests)
add_test(NAME Barrier COMMAND barrier_tests)
add_test(NAME Memory COMMAND memory_tests)
add_test(NAME Views COMMAND views_tests)
add_test(NAME StaticVector COMMAND static_vector_tests)
add_test(NAME Range COMMAND range_tests)
add_test(NAME Iterators COMMAND iterator_tests)

    
endif(${BUILD_TESTING})

option(INSTALL "Activates library installation" ON)

if(${INSTALL})
    install(TARGETS libstra DESTINATION lib)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/libstra DESTINATION include)
    install(FILES LibstraConfig.cmake DESTINATION lib/cmake)
endif(${INSTALL})
