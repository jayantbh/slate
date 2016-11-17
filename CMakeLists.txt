cmake_minimum_required(VERSION 3.5.1)
project(slate)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -lpanel -lncurses")
set(CMAKE_C_COMPILER "/usr/bin/gcc")


set(SOURCE_FILES
        source/slateui.c)

add_executable(slate.out ${SOURCE_FILES})

find_package( Curses REQUIRED )
include_directories( ${CURSES_INCLUDE_DIRS} )
target_link_libraries( slate.out ${CURSES_LIBRARIES} )