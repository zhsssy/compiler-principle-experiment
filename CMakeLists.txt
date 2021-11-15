cmake_minimum_required(VERSION 3.20)
project(compiler_principle_experiment)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

aux_source_directory(src DIR_SRC)

add_executable(compiler_principle_experiment src/lexical_analysis.cpp)
