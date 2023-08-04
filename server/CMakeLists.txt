cmake_minimum_required(VERSION 3.25)
project(server)

set(CMAKE_CXX_STANDARD 20)

add_executable(server src/main.cpp src/user.cpp src/chat_server.cpp)
include_directories(include ../common)

target_link_libraries(server PRIVATE znet)
target_precompile_headers(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/precompiled.h)

