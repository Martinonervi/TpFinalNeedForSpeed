// path: common/constants.h
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "opcodes.h"
#include "client_msg.h"
#include "server_msg.h"

#include "queue.h"
#include "socket.h"
#include "thread.h"

#define NITRO_ON_MSG "A car hit the nitro!"
#define NITRO_OFF_MSG "A car is out of juice."
#define ERR_GAME_FULL "La partida está llena"

#define RETURN_SUCCESS  0
#define RETURN_FAILURE  1

#define FRAMES_PER_CAR 16
#define FRAMES_PER_DIRECTION 8
#define SMALL_CAR 32
#define MEDIUM_CAR 40
#define LARGE_CAR 48
#define PIXELS_PER_METER 10
#define CARS_PATH  "../assets/cars/cars.png"
#define PEOPLE_PATH  "../assets/cars/cars.png"
#define SAN_ANDREAS_PATH  "../assets/cities/san_andreas.png"
#define LIBERTY_CITY_PATH  "../assets/cities/liberty_city.png"
#define VICE_CITY_PATH  "../assets/cities/vice_city.png"
#define EXPLOSION_PATH "../assets/effects/explosion.png"


using ID = std::uint32_t;

using CliMsgPtr = std::shared_ptr<CliMsg>;
using SrvMsgPtr = std::shared_ptr<SrvMsg>;

constexpr float PIXEL_TO_METER = 1.0f / 10.0f;   // 10 px = 1 m

//no veo ganancia en que Cmd sea una clase, cumple su funcion perfecta como struct
struct Cmd {
    ID client_id;
    std::shared_ptr<CliMsg> msg;
};

enum CarType {
    CAR_GREEN,
    CAR_RED,
    CAR_PORSCHE,
    CAR_LIGHT_BLUE,
    CAR_JEEP,
    CAR_PICKUP,
    CAR_LIMO,
    CAR_COUNT
};

enum MapType {
    MAP_LIBERTY,
    MAP_SAN_ANDREAS,
    MAP_VICE
};

enum EffectType {
    EXPLOSION,
    LOW_HEALTH
};

struct CarInfo {
    int yOffset;
    int width;
    int height;
};

struct GameMetadata {
    ID game_id;
    int players;
    bool started;
};

enum class EntityLayer {
    GROUND,
    BRIDGE
};