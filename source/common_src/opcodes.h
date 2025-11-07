#ifndef TPFINALNEEDFORSPEED_OPCODES_H
#define TPFINALNEEDFORSPEED_OPCODES_H

#include <cstdint>

using Op = std::uint8_t;
enum Opcode : Op {
    // no hace falta poner el opcode a cada vez es autoincremental
    // el enum pero se los dejo por si les sirve de algo
    Movement = 0x01,
    INIT_PLAYER = 0x02,
    NEW_PLAYER =0x03,
    JOIN_GAME = 0x04
};

enum err_code : Op {
    SUCCES = 0x00, // tal vez no va pero para no dejar el campo vacío si devolvemos un error
    UNKNOWN_ERR,
    FULL_GAME,
    INEXISTENT_GAME,
};


#endif
