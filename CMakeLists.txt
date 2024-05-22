# cmake lists for basic cpp project

cmake_minimum_required(VERSION 3.17)

set(PROJECT_NAME SwarDemo)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 20)

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
