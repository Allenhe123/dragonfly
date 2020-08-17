cmake_minimum_required(VERSION 3.10)
include(GNUInstallDirs)

project(msg-proto)

find_package(Protobuf REQUIRED)
if (NOT Protobuf_FOUND)
    message(FATAL_ERROR "Protobuf Not Found!")
else()
    message(STATUS "##Found Protobuf: ${PROTOBUF_INCLUDE_DIR} ${PROTOBUF_LIBRARIES}")
    include_directories(${PROTOBUF_INCLUDE_DIR})
    link_directories(${PROTOBUF_LIB_DIR})
endif (NOT Protobuf_FOUND)

file(GLOB PROTO_SRCS *.proto)
PROTOBUF_GENERATE_CPP(	PROTO_SRCS PROTO_HDRS ${PROTO_SRCS})

add_library(msg_proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(msg_proto ${Protobuf_LIBRARIES})
install(TARGETS msg_proto LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
