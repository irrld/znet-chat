cmake_minimum_required(VERSION 3.25)
project(znet-chat-parent)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(vendor/znet/znet)
add_subdirectory(vendor/znet/vendor/fmt)

add_subdirectory(client)
add_subdirectory(server)

