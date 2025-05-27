cmake_minimum_required(VERSION 3.12)

project(Bitcask VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

# Collect headers and sources
set(HEADERS include/db.hpp)
set(SOURCES src/db.cpp src/main.cpp)

# Main executable
add_executable(bitcask ${SOURCES} ${HEADERS})

# Include dir for all targets
target_include_directories(bitcask PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Test executable
add_executable(l1test test/l1cache_benchmark.cpp src/db.cpp ${HEADERS})
target_include_directories(l1test PRIVATE ${PROJECT_SOURCE_DIR}/include)

