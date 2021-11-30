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

#add_executable(compiler_principle_experiment ${DIR_SRC} src/ll1_programmer.cpp include/ll1_programmer.h)

# test lexical_analysis
#add_executable(compiler_principle_experiment src/lexical_analysis.cpp)

# test ll1_programmer
#add_executable(compiler_principle_experiment src/ll1_programmer.cpp)

# test operator_programmer
#add_executable(compiler_principle_experiment src/operator_programmer.cpp)