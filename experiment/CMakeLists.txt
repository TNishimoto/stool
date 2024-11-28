cmake_minimum_required(VERSION 3.21)
project(stool CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-D_INT128 -Wall -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-D_INT128 -Wall -Wextra")
endif(CPP_STATIC)

set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    message("Release Mode")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

#SDSL lib and include directory are here
#Please modify it appropriately.
find_library(SDSL_LIBRARY_DIR sdsl REQUIRED)
find_path(SDSL_INCLUDE_DIR sdsl REQUIRED)

LINK_DIRECTORIES(${SDSL_LIBRARY_DIR})
include_directories(${SDSL_INCLUDE_DIR})


FILE(GLOB SOURCES lib/*.cpp)

add_executable(rlbwt.out src/rlbwt_main.cpp ${SOURCES})
target_link_libraries(rlbwt.out sdsl)



#target_link_libraries(a.out fn)

