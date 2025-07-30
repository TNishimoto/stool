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

#[[

add_executable(elias_fano_test.out sources/elias_fano_test_main.cpp)
target_link_libraries(elias_fano_test.out sdsl)

add_executable(deque_test.out sources/deque_test_main.cpp)
target_link_libraries(deque_test.out sdsl)

add_executable(test.out sources/test_main.cpp)
target_link_libraries(test.out sdsl)

add_executable(rank_select_test.out sources/rank_select_test_main.cpp)
target_link_libraries(rank_select_test.out sdsl)

add_executable(fm_index_test.out sources/fm_index_test_main.cpp)
target_link_libraries(fm_index_test.out sdsl)


add_executable(lcp_interval_test.out sources/lcp_interval_test_main.cpp)
target_link_libraries(lcp_interval_test.out sdsl)

add_executable(rmq_test.out sources/rmq_test_main.cpp)
target_link_libraries(rmq_test.out sdsl)

add_executable(performance_test.out sources/performance_test_main.cpp)
target_link_libraries(performance_test.out sdsl)

add_executable(short_bit_vector_test.out sources/short_bit_vector_test_main.cpp)
target_link_libraries(short_bit_vector_test.out sdsl)

add_executable(short_elias_fano_test.out sources/short_elias_fano_test_main.cpp)
target_link_libraries(short_elias_fano_test.out sdsl)

add_executable(short_integer_vector_test.out sources/short_integer_vector_test_main.cpp)
target_link_libraries(short_integer_vector_test.out sdsl)

]]

add_executable(bit_deque_test.out sources/bit_deque_test_main.cpp)
target_link_libraries(bit_deque_test.out sdsl)

add_executable(integer_deque_test.out sources/integer_deque_test_main.cpp)
target_link_libraries(integer_deque_test.out sdsl)
