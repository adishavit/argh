project(argh)
cmake_minimum_required(VERSION 2.8)

add_executable(argh example.cpp)
add_executable(argh_tests argh_tests.cpp)

if(UNIX  OR CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-Wall -std=c++11" CACHE STRING "compile flags" FORCE )
endif() 
