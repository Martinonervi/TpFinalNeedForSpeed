#ifndef TPFINALNEEDFORSPEED_SRV_STARTING_GAME_H
#define TPFINALNEEDFORSPEED_SRV_STARTING_GAME_H
#include "server_msg.h"


class StartingGame : public SrvMsg {
public:
    StartingGame() = default;
    Op type() const override { return Opcode::STARTING_GAME; }
};
#endif
