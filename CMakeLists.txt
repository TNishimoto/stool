cmake_minimum_required(VERSION 3.21)
if(MY_SERVER)
set(HAVE_FLAG_SEARCH_PATHS_FIRST 0)
endif(MY_SERVER)

project(stool CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MY_SERVER)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_SYSROOT /opt/homebrew/Cellar/x86_64-unknown-linux-gnu/13.2.0/toolchain/x86_64-unknown-linux-gnu/sysroot)

set(CMAKE_EXE_LINKER_FLAGS -B/opt/homebrew/Cellar/x86_64-unknown-linux-gnu/13.2.0/toolchain/lib/gcc/x86_64-unknown-linux-gnu/13.2.0)
set(CMAKE_CXX_FLAGS "--target=x86_64-linux-gnu -fexceptions -m64 -D_INT128 -static")
include_directories(/opt/homebrew/Cellar/x86_64-unknown-linux-gnu/13.2.0/toolchain/x86_64-unknown-linux-gnu/include/c++/13.2.0/x86_64-unknown-linux-gnu)
include_directories(/opt/homebrew/Cellar/x86_64-unknown-linux-gnu/13.2.0/toolchain/x86_64-unknown-linux-gnu/include/c++/13.2.0)
link_directories(/opt/homebrew/Cellar/x86_64-unknown-linux-gnu/13.2.0/toolchain/lib/gcc/x86_64-unknown-linux-gnu/13.2.0)
else(MY_SERVER)
set(CMAKE_CXX_FLAGS "-m64  -D_INT128 -Wall -Wextra")
endif(MY_SERVER)


set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    message("Release Mode")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# Set PROJECT_IS_TOP_LEVEL to ON if if this is the top level project (not
# if this is included as a subproject of something else). Note that this is
# handled automatically for CMake >= 3.21.
if (CMAKE_VERSION VERSION_LESS 3.21)
    if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
        message("SET PROJECT_IS_TOP_LEVEL ON IN ${PROJECT_NAME}")
        set (PROJECT_IS_TOP_LEVEL ON)
    else()
        message("SET PROJECT_IS_TOP_LEVEL OFF IN ${PROJECT_NAME}")
    set (PROJECT_IS_TOP_LEVEL OFF)    
    endif()
endif ()

FILE(GLOB SOURCES lib/*.cpp)

INCLUDE_DIRECTORIES(include)
add_library(stool STATIC ${SOURCES})
target_link_libraries(stool stdc++)


#target_link_libraries(a.out fn)
