cmake_minimum_required(VERSION 3.1)

project(argh VERSION 1.3.1)
set(PROJECT_DESCRIPTION "A minimalist argument handler.")
set(PROJECT_HOMEPAGE_URL "https://github.com/adishavit/argh")

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
	string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")
	install(FILES "${CMAKE_CURRENT_LIST_DIR}/argh.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
	install(FILES "${CMAKE_CURRENT_LIST_DIR}/LICENSE" DESTINATION ${CMAKE_INSTALL_DOCDIR})
	install(FILES "${CMAKE_CURRENT_LIST_DIR}/README.md" DESTINATION ${CMAKE_INSTALL_DOCDIR})
	set(PackagingTemplatesDir "${CMAKE_CURRENT_LIST_DIR}/packaging")

	if(CMAKE_SYSTEM_NAME STREQUAL Linux)
	# this might be a bit too restrictive, since for other (BSD, ...) this might apply also
	# but this can be fixed later in extra pull requests from people on the platform
		install(FILES "${PackagingTemplatesDir}/argh-config.cmake" DESTINATION ${INSTALL_LIBDIR_ARCHIND}/cmake/argh)
		install(EXPORT arghTargets DESTINATION ${INSTALL_LIBDIR_ARCHIND}/cmake/argh)
	else()
		install(FILES "${PackagingTemplatesDir}/argh-config.cmake" DESTINATION CMake)
		install(EXPORT arghTargets DESTINATION CMake)
	endif()

	set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
	set(CPACK_PACKAGE_VENDOR "Argh developers")
	set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
	set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
	set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
	set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
	set(CPACK_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_MAINTAINER}")
	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
	set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

	set(CPACK_DEBIAN_PACKAGE_NAME "lib${PROJECT_NAME}-dev")
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6-dev")
	set(CPACK_DEBIAN_PACKAGE_SUGGESTS "cmake, pkg-config, pkg-conf")

	set(CPACK_RPM_PACKAGE_NAME "lib${PROJECT_NAME}-devel")
	set(CPACK_RPM_PACKAGE_SUGGESTS "${CPACK_DEBIAN_PACKAGE_SUGGESTS}")

	set(CPACK_DEB_COMPONENT_INSTALL ON)
	set(CPACK_RPM_COMPONENT_INSTALL ON)
	set(CPACK_NSIS_COMPONENT_INSTALL ON)
	set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")

	set(PKG_CONFIG_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")
	configure_file("${PackagingTemplatesDir}/pkgconfig.pc.in" "${PKG_CONFIG_FILE_NAME}" @ONLY)
	install(FILES "${PKG_CONFIG_FILE_NAME}"
		DESTINATION "${INSTALL_LIBDIR_ARCHIND}/pkgconfig"
	)

	include(CPack)
endif()
