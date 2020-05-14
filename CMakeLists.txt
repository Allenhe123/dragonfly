cmake_minimum_required(VERSION 3.10)
include(GNUInstallDirs)

project(df-project)

add_compile_options("-O2")

# include_directories(${OpenCV_INCLUDE_DIRS})
# include_directories(../bsnn/include)
# include_directories(../adas/include)
# include_directories(../include)
# include_directories(../cyberRT)

# include_directories(${CMAKE_BINARY_DIR})
# message(STATUS "##CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})
# #include_directories
# include_directories(${PROJECT_BINARY_DIR}/cyberRT)


AUX_SOURCE_DIRECTORY(. DIR_SRCS)
add_library(df SHARED ${DIR_SRCS})
target_link_libraries(df pthread)

install(TARGETS df DESTINATION ${CMAKE_INSTALL_LIBDIR})