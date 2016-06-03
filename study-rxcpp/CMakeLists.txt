# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (study-rxcpp)
include_directories("/Users/pandaxcl/Documents/pandaxcldev/rxcpp.git/Rx/v2/src")
set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")
# 指定生成目标
add_executable(kvo-test kvo-test.cpp kvo_extension.hpp)
