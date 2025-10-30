// path: common/constants.h
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "queue.h"
#include "socket.h"
#include "thread.h"

#define NITRO_ON_MSG "A car hit the nitro!"
#define NITRO_OFF_MSG "A car is out of juice."

#define RETURN_SUCCESS  0
#define RETURN_FAILURE  1

// Tipos personalizados
using Cars_W_Nitro = std::uint16_t;
using Op = std::uint8_t;
using ID = std::uint32_t;

struct MoveInfo {
    uint8_t accelerate;    // W / acelerar, 0 false, 1 true
    uint8_t brake;         // S / frenar, 0 false, 1 true
    int8_t steer;       // -1 izquierda, 0 recto, +1 derecha
    uint8_t nitro;  // 0 false, 1 true
};

struct CliMsg {
    Op event_type{0};
    MoveInfo movement{};
};

// Cambiar a mayus
enum Opcode : Op { Movement = 0x01, ClientMSG = 0x04, ServerMSG = 0x10, NitroON = 0x07,
                   NitroOFF = 0x08, Nitro = 0x09 };


struct Cmd {
    Opcode type;
    ID client_id{0};
    MoveInfo movimiento;
};


struct PlayerStateUpdate {
    uint16_t player_id;

    float x; //box2d usa metros
    float y;
    float angleRad;
};


struct SrvMsg {
    Opcode type{};
    PlayerStateUpdate posicion;
};

#define SMALL_CAR 32
#define MEDIUM_CAR 40
#define LARGE_CAR 48

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

#define FRAMES_PER_CAR 16
#define FRAMES_PER_DIRECTION 8

struct CarInfo {
    int yOffset;
    int width;
    int height;
};