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

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${zoo_SOURCE_DIR}/inc)
