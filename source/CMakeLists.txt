cmake_minimum_required(VERSION 3.16)
project(TpFinalNeedForSpeed LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Cliente
file(GLOB CLIENT_SOURCES
        "client_src/*.cpp"
        "common_src/*.cpp"
)

list(APPEND CLIENT_SOURCES "client_main.cpp")

add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client Qt5::Widgets Qt5::Core pthread)

# Servidor
file(GLOB SERVER_SOURCES
        "server_src/*.cpp"
        "common_src/*.cpp"
)

list(APPEND SERVER_SOURCES "server_main.cpp")

add_executable(server ${SERVER_SOURCES})
target_link_libraries(server pthread)