cmake_minimum_required(VERSION 2.8)
project(stool CXX)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++11 -m64  -D_INT128 -Wall -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++11 -m64  -D_INT128 -Wall -Wextra")
endif(CPP_STATIC)

set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    message("Release Mode")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

FILE(GLOB SOURCES lib/*.cpp)

INCLUDE_DIRECTORIES(include)
add_library(stool STATIC ${SOURCES})


#target_link_libraries(a.out fn)
