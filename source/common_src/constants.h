// path: common/constants.h
#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <arpa/inet.h>

#include "cli_msg/client_msg.h"
#include "srv_msg/server_msg.h"

#include "opcodes.h"
#include "queue.h"
#include "socket.h"
#include "thread.h"

#define NITRO_ON_MSG "A car hit the nitro!"
#define NITRO_OFF_MSG "A car is out of juice."
#define ERR_GAME_FULL "La partida está llena"

#define RETURN_SUCCESS  0
#define RETURN_FAILURE  1

#define FPS 60.0

#define FRAMES_PER_CAR 16
#define FRAMES_PER_DIRECTION 8
#define SMALL_CAR 32
#define MEDIUM_CAR 40
#define LARGE_CAR 48
#define PIXELS_PER_METER 10
#define WORLD_WIDTH 4640.0
#define WORLD_HEIGHT 4672.0

// OTRO ARCHIVO
#define CARS_PATH  "../assets/cars/cars.png"
#define PEOPLE_PATH  "../assets/cars/cars.png"
#define SAN_ANDREAS_PATH  "../assets/cities/san_andreas.png"
#define LIBERTY_CITY_PATH  "../assets/cities/liberty_city.png"
#define VICE_CITY_PATH  "../assets/cities/vice_city.png"
#define EXPLOSION_PATH "../assets/effects/explosion.png"
#define SPEEDOMETER_PATH "../assets/cars/speedometer.png"
#define CHECKPOINT_PATH "../assets/extras/checkpoint.png"
#define HEALTH_NITRO_PATH "../assets/extras/health-and-nitro.png"
#define SWORD_SHIELD_PATH "../assets/extras/sword-and-shield.png"
#define START_BACK_PATH "../assets/extras/start-background.png"
#define UPGRADES_PATH "../assets/extras/upgrades.png"
#define LIBERTY_OVER_PATH "../assets/cities/liberty_city_over.png"
#define NPCS_PATH "../assets/people/people.png"
#define FONT_PATH "../client_src/lobby/resources/fonts/pressstart2p.ttf"

// #define FONT_PATH "../assets/fonts/pixel_font.ttf"

using ID = std::uint32_t;

using CliMsgPtr = std::shared_ptr<CliMsg>;
using SrvMsgPtr = std::shared_ptr<SrvMsg>;

constexpr float PIXEL_TO_METER = 1.0f / 10.0f;   // 10 px = 1 m

struct UpgradeDef {
    Upgrade type;
    float value;
    float penaltySec;
};


//no veo ganancia en que Cmd sea una clase, cumple su funcion perfecta como struct
struct Cmd {
    ID client_id;
    std::shared_ptr<CliMsg> msg;
};

enum CarType : uint8_t{
    CAR_GREEN,
    CAR_RED,
    CAR_PORSCHE,
    CAR_LIGHT_BLUE,
    CAR_JEEP,
    CAR_PICKUP,
    CAR_LIMO,
    CAR_COUNT
};

enum CarState : uint8_t{
    ALIVE,
    LOW_HEALTH,
    EXPLODING,
    DESTROYED
};

enum MapType : uint8_t{
    MAP_LIBERTY,
    MAP_SAN_ANDREAS,
    MAP_VICE
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



// Helpers para el protocolo

inline uint32_t encodeFloatBE(float value) {
    int32_t fixed = static_cast<int32_t>(std::lround(value * 100.0f));
    uint32_t as_u32 = static_cast<uint32_t>(fixed);
    return htonl(as_u32);
}

inline float decodeFloatBE(uint32_t be_value) {
    uint32_t as_u32 = ntohl(be_value);
    int32_t fixed = static_cast<int32_t>(as_u32);
    return static_cast<float>(fixed) / 100.0f;
}