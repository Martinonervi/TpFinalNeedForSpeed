
#ifndef TPFINALNEEDFORSPEED_CLI_START_GAME_H
#define TPFINALNEEDFORSPEED_CLI_START_GAME_H

#include "client_msg.h"

class StartGame : public CliMsg {

public:
    StartGame() = default;
    Op type() const override { return Opcode::START_GAME; }

};

#endif
