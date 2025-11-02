#ifndef TPFINALNEEDFORSPEED_OPCODES_H
#define TPFINALNEEDFORSPEED_OPCODES_H

#include <cstdint>

using Op = std::uint8_t;
enum Opcode : Op {
    Movement = 0x01, INIT_PLAYER = 0x02, NEW_PLAYER =0x03
};


#endif
