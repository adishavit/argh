project(argh)
cmake_minimum_required(VERSION 2.8)

add_executable(argh example.cpp)
add_executable(argh_tests argh_tests.cpp)

if(UNIX OR CMAKE_COMPILER_IS_GNUCXX)
   add_definitions("-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -std=c++11")
else(MSVC)
   add_definitions("/W4 /WX")
 endif() 
