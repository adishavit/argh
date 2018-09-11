project(argh)
cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

option(BUILD_TESTS "Build tests. Uncheck for install only runs" ON)
option(BUILD_EXAMPLES "Build examples. Uncheck for install only runs" ON)

if(BUILD_EXAMPLES)
	add_executable(argh_example example.cpp)
endif()
if(BUILD_TESTS)
	add_executable(argh_tests   argh_tests.cpp)
endif()

add_library(argh INTERFACE)
target_include_directories(argh INTERFACE $<BUILD_INTERFACE:"${CMAKE_CURRENT_LIST_DIR}"> $<INSTALL_INTERFACE:include>)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT argh_tests)

if(BUILD_EXAMPLES OR BUILD_TESTS)
	if(UNIX OR CMAKE_COMPILER_IS_GNUCXX)
	   add_definitions("-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")
	else(MSVC)
	   add_definitions("/W4 /WX")
	 endif() 
endif()

install(TARGETS argh EXPORT arghTargets)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/argh.h" DESTINATION include)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/LICENSE" DESTINATION license)
install(FILES argh-config.cmake DESTINATION CMake)
install(EXPORT arghTargets DESTINATION CMake)
