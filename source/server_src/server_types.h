// path: server_src/server_types.h
#pragma once
#include <cstdint>
#include <memory>

#include "../common_src/constants.h"
#include "../common_src/printer.h"
#include "../common_src/queue.h"

#define NITRO_TICKS 12
#define TICK_MS 250

namespace serv_types {
using ID = constants::ID;

// Mensaje interno Receiver -> GameLoop
enum class CommandType : std::uint8_t { Nitro = 0x01 , MoveRequest = 0x02};

struct Cmd {
    constants::Opcode type;
    ID client_id{0};
    constants::MoveInfo movimiento;
};

// Tipos server-side
using SendQ = Queue<constants::CliMsg>;
using SendQPtr = std::shared_ptr<SendQ>;
using gameLoopQueue = Queue<constants::Cmd>;

}  // namespace serv_types
