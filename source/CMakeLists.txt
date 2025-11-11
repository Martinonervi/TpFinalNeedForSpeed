cmake_minimum_required(VERSION 3.16)
project(TpFinalNeedForSpeed LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Qt ---

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if (APPLE AND NOT CMAKE_PREFIX_PATH)
    list(APPEND CMAKE_PREFIX_PATH
            "/opt/homebrew/opt/qt"
            "/opt/homebrew/opt/qt@5"
            "/usr/local/opt/qt"
            "/usr/local/opt/qt@5"
    )
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)


# SDL2 + SDL2pp
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

# Box2D
FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v3.1.1
)
FetchContent_MakeAvailable(box2d)


set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
# yaml-cpp
FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG 0.8.0
)
FetchContent_MakeAvailable(yaml-cpp)


# --- Librería common ---
file(GLOB COMMON_SOURCES "common_src/*.cpp")
add_library(common STATIC ${COMMON_SOURCES}
        common_src/requestgame.h
        common_src/joingame.h
        common_src/metadatagames.h)
target_include_directories(common PUBLIC ${libSDL2pp_SOURCE_DIR}/include)
target_link_libraries(common PUBLIC SDL2pp::SDL2pp pthread box2d::box2d)


file(GLOB_RECURSE CLIENT_SOURCES "client_src/*.cpp")
list(APPEND CLIENT_SOURCES "client_main.cpp")
add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Core
        common
)

# --- Servidor ---
file(GLOB SERVER_SOURCES "server_src/*.cpp")
list(APPEND SERVER_SOURCES "server_main.cpp")
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE common pthread yaml-cpp)