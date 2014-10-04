cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -pthread")

project(sandbox)
add_executable(sandbox main.cpp)
