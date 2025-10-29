cmake_minimum_required(VERSION 3.16)
project(TpFinalNeedForSpeed LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt, no lo usamos todavia, no sirve sin descarga
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# SDL, descarga automatica usando el repo
include(FetchContent)
FetchContent_Declare(
        libSDL2pp
        GIT_REPOSITORY https://github.com/libSDL2pp/libSDL2pp.git
        GIT_TAG 0.18.1
)
set(SDL2PP_WITH_IMAGE ON)
set(SDL2PP_WITH_TTF ON)
set(SDL2PP_WITH_MIXER ON)
FetchContent_MakeAvailable(libSDL2pp)

file(GLOB COMMON_SOURCES "common_src/*.cpp")
add_library(common STATIC ${COMMON_SOURCES})
target_include_directories(common PUBLIC ${libSDL2pp_SOURCE_DIR}/include)
target_link_libraries(common PUBLIC SDL2pp::SDL2pp pthread)

file(GLOB CLIENT_SOURCES "client_src/*.cpp")
list(APPEND CLIENT_SOURCES "client_main.cpp")
add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client PRIVATE Qt5::Widgets Qt5::Core common)

file(GLOB SERVER_SOURCES "server_src/*.cpp")
list(APPEND SERVER_SOURCES "server_main.cpp")
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE common pthread)
