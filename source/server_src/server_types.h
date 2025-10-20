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
enum class CommandType : std::uint8_t { Nitro = 0x01 };

struct Cmd {
    CommandType type{CommandType::Nitro};
    ID client_id{0};
};

// Tipos server-side
using SendQ = Queue<constants::OutMsg>;
using SendQPtr = std::shared_ptr<SendQ>;
using gameLoopQueue = Queue<Cmd>;

}  // namespace serv_types
