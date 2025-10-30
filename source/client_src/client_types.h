#pragma once

#include "../common_src/constants.h"
#include "../common_src/printer.h"

#define NITRO_CMD "nitro"
#define EXIT_CMD "exit"
#define ACCELERATE "w"


inline const std::unordered_map<std::string_view, Op> cmdToOp = {
        {NITRO_CMD, static_cast<Op>(Opcode::Nitro)},
        {ACCELERATE, static_cast<Op>(Opcode::Movement)},
};


