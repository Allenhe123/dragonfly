cmake_minimum_required(VERSION 3.10)
include(GNUInstallDirs)
project(sample)

#include_directories(${CMAKE_CURRENT_BINARY_DIR})
#message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${CMAKE_BINARY_DIR}/src/proto)

add_executable(sample sample.cpp)
target_link_libraries(sample df profiler)

install(TARGETS sample DESTINATION ${CMAKE_INSTALL_BINDIR})
