cmake_minimum_required(VERSION 3.10)
include(GNUInstallDirs)

project(df-unit-test)

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

find_package(GTest REQUIRED)
if (NOT GTest_FOUND)
    message(FATAL_ERROR "GTest Not Found")
else ()
    message(STATUS "##Found GTest: ${GTEST_INCLUDE_DIR} ${GTEST_LIBRARIES}")
    include_directories(${GTEST_INCLUDE_DIR})
endif (NOT GTest_FOUND)

include_directories(${CMAKE_BINARY_DIR}/config)

# message(STATUS "DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/asio/include)

include_directories(${CMAKE_BINARY_DIR}/src/proto)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)
add_executable(asio-test asio_test.cc)
add_executable(publish publish.cc)
add_executable(subscribe subscribe.cc)

target_link_libraries(asio-test pthread ${Protobuf_LIBRARIES} df_proto msg_proto ${GTEST_LIBRARIES})
target_link_libraries(publish pthread ${Protobuf_LIBRARIES} df_proto msg_proto ${GTEST_LIBRARIES})
target_link_libraries(subscribe pthread ${Protobuf_LIBRARIES} df_proto msg_proto ${GTEST_LIBRARIES})

install(TARGETS asio-test DESTINATION ${CMAKE_INSTALL_LIBDIR})