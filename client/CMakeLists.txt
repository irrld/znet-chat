cmake_minimum_required(VERSION 3.26)
project(client)

set(CMAKE_CXX_STANDARD 23)

add_executable(client src/app.cpp src/client.cpp)
target_link_libraries(client PRIVATE wiesel znet)
include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ../common)

target_include_assets(client ${CMAKE_CURRENT_SOURCE_DIR}/assets)