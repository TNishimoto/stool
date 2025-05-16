cmake_minimum_required(VERSION 3.21)
project(stool CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif(CPP_STATIC)

set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

#SDSL lib and include directory are here
#Please modify it appropriately.
find_library(SDSL_LIBRARY_DIR sdsl REQUIRED)
find_path(SDSL_INCLUDE_DIR sdsl REQUIRED)

LINK_DIRECTORIES(${SDSL_LIBRARY_DIR})
include_directories(${SDSL_INCLUDE_DIR})

add_executable(elias_fano_test.out elias_fano_test_main.cpp)
target_link_libraries(elias_fano_test.out sdsl)

add_executable(deque_test.out deque_test_main.cpp)
target_link_libraries(deque_test.out sdsl)

add_executable(test.out test_main.cpp)
target_link_libraries(test.out sdsl)

add_executable(rank_select_test.out rank_select_test_main.cpp)
target_link_libraries(rank_select_test.out sdsl)

add_executable(fm_index_test.out fm_index_test_main.cpp)
target_link_libraries(fm_index_test.out sdsl)


add_executable(lcp_interval_test.out lcp_interval_test_main.cpp)
target_link_libraries(lcp_interval_test.out sdsl)

add_executable(rmq_test.out rmq_test_main.cpp)
target_link_libraries(rmq_test.out sdsl)

add_executable(performance_test.out performance_test_main.cpp)
target_link_libraries(performance_test.out sdsl)
