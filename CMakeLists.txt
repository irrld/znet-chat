cmake_minimum_required(VERSION 3.25)
project(znet-chat-parent)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor/znet/znet)
set(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PARENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(VENDOR_DIR ${PARENT_DIR}/vendor/Wiesel/vendor)

add_subdirectory(vendor/wiesel)

add_subdirectory(server)
add_subdirectory(client)

