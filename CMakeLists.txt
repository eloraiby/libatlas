project(atlas C)
cmake_minimum_required(VERSION 2.8)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")
    endif ()
else ()
    set (CMAKE_C_STANDARD 99)
endif ()

set(SRC_FILES
        stb/stb_rect_pack.c
        image.c)
set(HEADER_FILES
        stb/stb_rect_pack.h
        atlas.h)

include_directories(..)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${HEADER_FILES})
add_library(${PROJECT_NAME}s STATIC ${SRC_FILES} ${HEADER_FILES})

add_executable(${PROJECT_NAME}-test ${SRC_FILES} main.c)
