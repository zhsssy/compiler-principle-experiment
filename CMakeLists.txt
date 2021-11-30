cmake_minimum_required(VERSION 3.20)
project(compiler_principle_experiment)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

#aux_source_directory(src DIR_SRC)

add_subdirectory(src)

# set debug mode ,output info
#SET(CMAKE_BUILD_TYPE DEBUG)

IF (CMAKE_BUILD_TYPE STREQUAL DEBUG)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF ()
