project(argh)
cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

add_executable(argh       example.cpp)
add_executable(argh_tests argh_tests.cpp)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT argh_tests)

if(UNIX OR CMAKE_COMPILER_IS_GNUCXX)
   add_definitions("-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")
else(MSVC)
   add_definitions("/W4 /WX")
 endif() 
