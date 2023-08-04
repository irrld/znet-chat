cmake_minimum_required(VERSION 3.26)
project(client)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

set(SOURCES
        src/connectwindow.cpp
        src/chatwindow.cpp
        src/client.cpp
        src/main.cpp
        src/userlistwidget.cpp
        src/chatwidget.cpp
        src/wrappedlistitemdelegate.cpp
)
set(HEADERS
        include/connectwindow.h
        include/chatwindow.h
        include/userlistwidget.h
        include/chatwidget.h
        include/wrappedlistitemdelegate.h
)

include_directories(include ../common)

add_executable(client
        ${SOURCES}
        ${HEADERS}
)

target_link_libraries(client
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        znet
)


