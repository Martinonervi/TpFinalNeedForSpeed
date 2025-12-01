#ifndef TPFINALNEEDFORSPEED_CLI_CHEAT_REQUEST_H
#define TPFINALNEEDFORSPEED_CLI_CHEAT_REQUEST_H

#include "../constants.h"
#include "client_msg.h"

class CheatRequest : public CliMsg {

public:
    CheatRequest(Cheat cheat): cheat(cheat) {};
    Op type() const override { return Opcode::REQUEST_CHEAT; };
    Cheat getCheat() const { return cheat; }
private:
    Cheat cheat;
};


#endif
