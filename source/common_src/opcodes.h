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
    JOIN_GAME = 0x04,
    REQUEST_GAMES = 0X05,
    COLLISION = 0X06,
    CHECKPOINT_DEF = 0X07,
    CHECKPOINT_HIT = 0X08,
    CLIENT_DISCONNECT = 0X09,
    STATS = 0x10,
    CURRENT_INFO = 0x11,
    TIME = 0x12,
    UPGRADE_LOGIC = 0X13,
    UPGRADE_REQUEST = 0x14,
    UPGRADE_SEND = 0x15,
    START_GAME      = 0x16,
    RECOMMENDED_PATH = 0x17,
    SEND_PLAYER = 0x18,
    CAR_SELECT = 0x19,
    SRV_DISCONNECTION = 0x20,
    REQUEST_CHEAT = 0x21,
};

enum err_code : Op {
    SUCCES = 0x00, // tal vez no va pero para no dejar el campo vacío si devolvemos un error
    UNKNOWN_ERR,
    FULL_GAME,
    INEXISTENT_GAME,
    STARTED_GAME
};

enum Upgrade : Op {
    NONE = 0x01,
    ENGINE_FORCE = 0x02,
    HEALTH = 0x03,
    SHIELD = 0x04,
    DAMAGE = 0x05
};

enum Cheat : Op {
    HEALTH_CHEAT = 0x01, //t
    WIN_RACE_CHEAT = 0x02, //r
    FREE_SPEED_CHEAT = 0x03, //u
    LOST_RACE_CHEAT = 0x04, //c
    NEXT_CHECKPOINT_CHEAT = 0x05, //o
};

#endif
