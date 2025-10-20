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

// Protocolo
struct OutMsg {
    Cars_W_Nitro cars_with_nitro{0};  // conteo
    Op event_type{0};                 // 0x07 on, 0x08 off
};

enum Opcode : Op { ClientMSG = 0x04, ServerMSG = 0x10, NitroON = 0x07, NitroOFF = 0x08 };

struct Cmd {
    enum Type { Nitro } type;
    ID client_id{0};
};

}  // namespace constants
