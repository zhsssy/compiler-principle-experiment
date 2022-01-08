cmake_minimum_required(VERSION 3.20)
project(compiler_principle_experiment)

set(CMAKE_CXX_STANDARD 20)

# windows 下 MINGW 问题，必须设置编码解决
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")
endif ()

# set debug mode ,output debug info
#SET(CMAKE_BUILD_TYPE DEBUG)
IF (CMAKE_BUILD_TYPE STREQUAL DEBUG)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF ()

include_directories(include)

add_subdirectory(src)
