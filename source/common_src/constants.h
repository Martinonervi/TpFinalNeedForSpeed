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

namespace constants {

#define NITRO_ON_MSG "A car hit the nitro!"
#define NITRO_OFF_MSG "A car is out of juice."

enum : int { RETURN_SUCCESS = 0, RETURN_FAILURE = 1 };

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
    Cars_W_Nitro cars_with_nitro{0};
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
    uint16_t player_id;  // quién es este auto (el server lo sabe)
    uint32_t tick;       // número de tick simulado del server

    float x;
    float y;
    float angle_deg;

    float vx;
    float vy;
};


struct SrvMsg {
    Opcode type{};
    Cars_W_Nitro cars_with_nitro{0};
    PlayerStateUpdate posicion;
};

}  // namespace constants
