cmake_minimum_required(VERSION 3.21)
project(stool_performance_test CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif(CPP_STATIC)

set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG  -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG  -Rpass=loop-vectorize")

INCLUDE_DIRECTORIES(../modules)
INCLUDE_DIRECTORIES(../modules/sdsl-lite/include)

add_executable(vector_collection_benchmark sources/vector_collection_benchmark.cpp)
