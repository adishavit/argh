cmake_minimum_required(VERSION 3.1)

project(argh)

set(CMAKE_CXX_STANDARD 11)

# Check if argh is being used directly or via add_subdirectory
set(ARGH_MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(ARGH_MASTER_PROJECT ON)
endif()

option(BUILD_TESTS "Build tests. Uncheck for install only runs"
       ${ARGH_MASTER_PROJECT})
option(BUILD_EXAMPLES "Build examples. Uncheck for install only runs"
       ${ARGH_MASTER_PROJECT})

if (CMAKE_CXX_COMPILER_ID MATCHES "(Clang|GNU)")
	list(APPEND flags "-Wall" "-Wextra" "-Wshadow" "-Wnon-virtual-dtor" "-pedantic")
elseif(MSVC)
	list(APPEND flags "/W4" "/WX")
endif()

if(BUILD_EXAMPLES)
	add_executable(argh_example example.cpp)
	target_compile_options(argh_example PRIVATE ${flags})
endif()
if(BUILD_TESTS)
	add_executable(argh_tests   argh_tests.cpp)
	target_compile_options(argh_tests PRIVATE ${flags})
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT argh_tests)
endif()

add_library(argh INTERFACE)
target_include_directories(argh INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> $<INSTALL_INTERFACE:include>)

if(ARGH_MASTER_PROJECT)
	install(TARGETS argh EXPORT arghTargets)

	include(GNUInstallDirs)
	install(FILES "${CMAKE_CURRENT_LIST_DIR}/argh.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
	install(FILES "${CMAKE_CURRENT_LIST_DIR}/LICENSE" DESTINATION ${CMAKE_INSTALL_DOCDIR})
	install(FILES "${CMAKE_CURRENT_LIST_DIR}/README.md" DESTINATION ${CMAKE_INSTALL_DOCDIR})

	if(APPLE OR CMAKE_SYSTEM_NAME STREQUAL Linux)
	# this might be a bit too restrictive, since for other (BSD, ...) this might apply also
	# but this can be fixed later in extra pull requests from people on the platform
		install(FILES argh-config.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/argh)
		install(EXPORT arghTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/argh)
	else()
		install(FILES argh-config.cmake DESTINATION CMake)
		install(EXPORT arghTargets DESTINATION CMake)
	endif()
endif()
