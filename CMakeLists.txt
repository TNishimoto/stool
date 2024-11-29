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

if (PROJECT_IS_TOP_LEVEL)
INCLUDE_DIRECTORIES(include)
add_library(stool STATIC ${SOURCES})
target_link_libraries(stool sdsl)

add_executable(elias_fano_test.out test/elias_fano_test_main.cpp ${SOURCES})
target_link_libraries(elias_fano_test.out stool)

add_executable(deque_test.out test/deque_test_main.cpp ${SOURCES})
target_link_libraries(deque_test.out stool)

add_executable(test.out test/test_main.cpp ${SOURCES})
target_link_libraries(test.out stool)

add_executable(rank_select_test.out test/rank_select_test_main.cpp ${SOURCES})
target_link_libraries(rank_select_test.out stool)


endif()


#target_link_libraries(a.out fn)

