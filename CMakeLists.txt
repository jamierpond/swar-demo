# cmake lists for basic cpp project

cmake_minimum_required(VERSION 3.17)

set(PROJECT_NAME SwarDemo)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 23)

set(USING_CLANG OFF)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(USING_CLANG ON)
endif ()

if (USING_CLANG)
  message(STATUS "Using Clang compiler")
  set(HAVE_STD_REGEX ON)
  set(RUN_HAVE_STD_REGEX 1)
endif ()

option(CMAKE_BUILD_TYPE "Default build type" Release)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include(FetchContent)

FetchContent_Declare(
        zoo
        GIT_REPOSITORY https://github.com/thecppzoo/zoo.git
        GIT_TAG master
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
)
FetchContent_Populate(zoo)
FetchContent_MakeAvailable(zoo)

set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark
        GIT_TAG main
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
)
FetchContent_Populate(benchmark)
FetchContent_MakeAvailable(benchmark)
add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})

# https://github.com/google/benchmark

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${zoo_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} PUBLIC ${zoo_SOURCE_DIR}/benchmark)

message(STATUS "BENCHMARK_DOWNLOAD_DEPENDENCIES: ${BENCHMARK_DOWNLOAD_DEPENDENCIES}")
target_link_libraries(${PROJECT_NAME} benchmark::benchmark)

set(COMPILER_FLAGS "-O3 -march=native")

if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  # assume avx2 is available
  message(STATUS "AVX2 is available")
  set(COMPILER_FLAGS "${COMPILER_FLAGS} -mavx2")
endif ()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
  message(STATUS "ARM processor detected")
  set(COMPILER_FLAGS "${COMPILER_FLAGS} -mfpu=neon")
endif ()

message(STATUS "Compiler flags: ${COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COMPILER_FLAGS}")


