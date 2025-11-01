#ifndef TPFINALNEEDFORSPEED_OPCODES_H
#define TPFINALNEEDFORSPEED_OPCODES_H

#include <cstdint>

using Op = std::uint8_t;
enum Opcode : Op {
    Movement = 0x01, ClientMSG = 0x04, ServerMSG = 0x10,
    NitroON = 0x07, NitroOFF = 0x08, Nitro = 0x09
};


#endif
