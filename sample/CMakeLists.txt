cmake_minimum_required(VERSION 3.10)
include(GNUInstallDirs)
project(sample)

add_executable(sample sample.cpp)
target_link_libraries(sample /mnt/e/work/dragonfly/build/libdf.so)

install(TARGETS sample DESTINATION ${CMAKE_INSTALL_BINDIR})