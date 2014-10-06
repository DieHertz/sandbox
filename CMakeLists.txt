cmake_minimum_required(VERSION 2.8)
project(sandbox CXX)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -g")
if (NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif (NOT APPLE)

include_directories("${PROJECT_SOURCE_DIR}")
add_subdirectory(net)

add_executable(sandbox main.cpp)
target_link_libraries(sandbox net)
