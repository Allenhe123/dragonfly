cmake_minimum_required(VERSION 3.10)
include(GNUInstallDirs)

project(dragonfly-project)

add_compile_options("-O2")
set(CMAKE_CXX_STANDARD 17)
if (NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE "Release")
endif()

add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(sample)
add_subdirectory(tests)

install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/df FILES_MATCHING PATTERN "*.h")
