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
    bool accelerate;    // W / acelerar
    bool brake;         // S / frenar
    int8_t steer;       // -1 izquierda, 0 recto, +1 derecha
    bool nitro;
};

// Protocolo
struct CliMsg {
    Cars_W_Nitro cars_with_nitro{0};  // conteo
    Op event_type{0};
    MoveInfo movement{};
};

enum Opcode : Op { Movement = 0x01, ClientMSG = 0x04, ServerMSG = 0x10, NitroON = 0x07,
                   NitroOFF = 0x08, Nitro = 0x09 };

/*
struct Cmd {
    enum Type { Nitro } type;
    ID client_id{0};
};
*/

struct Cmd {
    Opcode type;
    ID client_id{0};
    MoveInfo movimiento;
};


}  // namespace constants
