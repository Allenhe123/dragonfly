cmake_minimum_required(VERSION 3.10)
include(GNUInstallDirs)

project(df-project)

add_compile_options("-O2")

AUX_SOURCE_DIRECTORY(. DIR_SRCS)
add_library(df SHARED ${DIR_SRCS})
target_link_libraries(df pthread)

install(TARGETS df DESTINATION ${CMAKE_INSTALL_LIBDIR})