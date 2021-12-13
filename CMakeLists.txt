cmake_minimum_required(VERSION 3.20)
project(compiler_principle_experiment)

set(CMAKE_CXX_STANDARD 20)

# windows 下 MINGW 问题
# 如果调用 wsl 内 c compiler 没有此bug
# 必须设置编码

if(WIN)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")
endif()
include_directories(include)

#aux_source_directory(src DIR_SRC)

add_subdirectory(src)

# set debug mode ,output info
#SET(CMAKE_BUILD_TYPE DEBUG)

IF (CMAKE_BUILD_TYPE STREQUAL DEBUG)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF ()
