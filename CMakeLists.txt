# cmake lists for basic cpp project

cmake_minimum_required(VERSION 3.17)

project(SwarDemo)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        zoo
        GIT_REPOSITORY https://github.com/thecppzoo/zoo.git
        GIT_TAG jp/add-cmake
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
)

FetchContent_Populate(zoo)
FetchContent_MakeAvailable(zoo)
add_subdirectory(${zoo_SOURCE_DIR} ${zoo_BINARY_DIR})

add_executable(SwarDemo main.cpp)
