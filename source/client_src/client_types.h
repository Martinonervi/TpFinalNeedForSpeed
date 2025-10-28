#pragma once

#include "../common_src/constants.h"
#include "../common_src/printer.h"

#define NITRO_CMD "nitro"
#define READ_CMD "read"
#define EXIT_CMD "exit"

#define ACCELERATE "w"

namespace client_types{
using constants::RETURN_FAILURE;
using constants::RETURN_SUCCESS;

using Op = std::uint8_t;

inline const std::unordered_map<std::string_view, Op> cmdToOp = {
        {NITRO_CMD, static_cast<Op>(constants::Opcode::ClientMSG)},
        {ACCELERATE, static_cast<Op>(constants::Opcode::Movement)},
};

}
