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

# --- SDL2 ---

find_package(SDL2 REQUIRED)

include_directories(
        ${SDL2_INCLUDE_DIRS}
)

# --- SDL2pp (instalada a mano en /usr/local, versión 0.8.0) ---

find_library(SDL2PP_LIBRARY SDL2pp
        PATHS /usr/local/lib /usr/lib
)

if(NOT SDL2PP_LIBRARY)
    message(FATAL_ERROR "SDL2pp library not found")
endif()

# --- yaml-cpp ---

find_package(yaml-cpp REQUIRED)

if (TARGET yaml-cpp::yaml-cpp)
    set(YAML_CPP_TARGET yaml-cpp::yaml-cpp)
else()
    set(YAML_CPP_TARGET yaml-cpp)
endif()

# --- Box2D v3 (FetchContent) ---

include(FetchContent)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v3.1.1
)
FetchContent_MakeAvailable(box2d)

# --- Librería common ---

file(GLOB COMMON_SOURCES "common_src/*.cpp")

add_library(common STATIC
        ${COMMON_SOURCES}
)

target_link_libraries(common
        PUBLIC
        ${SDL2PP_LIBRARY}      # libSDL2pp.so
        ${SDL2_LIBRARIES}      # -lSDL2 (por SDL_MapRGB y cia)
        box2d::box2d
        pthread
)

# --- Cliente ---

file(GLOB_RECURSE CLIENT_SOURCES "client_src/*.cpp")
list(APPEND CLIENT_SOURCES "client_main.cpp")

set(CLIENT_RESOURCES
        client_src/lobby/lobby_resources.qrc
        client_src/postgame/postgame_resources.qrc
        common_src/srv_msg/srv_time_left.h
)

add_executable(client
        ${CLIENT_SOURCES}
        ${CLIENT_RESOURCES}
)

target_link_libraries(client
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Core
        common
        ${SDL2PP_LIBRARY}
        ${SDL2_LIBRARIES}
        SDL2_ttf
)

# --- Servidor ---

file(GLOB_RECURSE SERVER_SOURCES "server_src/*.cpp")
list(APPEND SERVER_SOURCES "server_main.cpp")

add_executable(server ${SERVER_SOURCES})

target_link_libraries(server
        PRIVATE
        common
        pthread
        ${YAML_CPP_TARGET}
)
