
if(TARGET libstra::libstra)
    return()
endif(TARGET libstra::libstra)

add_library(libstra::libstra INTERFACE IMPORTED)
target_include_directories(libstra::libstra INTERFACE 
    ${CMAKE_CURRENT_LIST_DIR}/include
)
