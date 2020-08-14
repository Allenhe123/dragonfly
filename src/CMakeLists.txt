cmake_minimum_required(VERSION 3.10)
include(GNUInstallDirs)

project(df-project)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE")
add_compile_options(-DASIO_STANDALONE)

find_package(Protobuf REQUIRED)
if (NOT Protobuf_FOUND)
    message(FATAL_ERROR "Protobuf Not Found!")
else()
    message(STATUS "##Found Protobuf: ${PROTOBUF_INCLUDE_DIR} ${PROTOBUF_LIBRARIES}")
    include_directories(${PROTOBUF_INCLUDE_DIR})
    link_directories(${PROTOBUF_LIB_DIR})
endif (NOT Protobuf_FOUND)

include_directories(${CMAKE_BINARY_DIR}/config)

# message(STATUS "DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/asio/include)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)
add_library(df SHARED ${DIR_SRCS})
target_link_libraries(df pthread ${Protobuf_LIBRARIES} df_proto)

install(TARGETS df DESTINATION ${CMAKE_INSTALL_LIBDIR})