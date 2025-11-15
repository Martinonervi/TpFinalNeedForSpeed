//
// Created by Felipe Fialayre on 03/11/2025.
//

#ifndef JOINGAME_H
#define JOINGAME_H
#include "server_msg.h"

class JoinGame: public SrvMsg {
public:
    JoinGame(bool could_join, err_code code): joined(could_join), exit_status(code){};

    err_code getExitStatus() const {return exit_status;}
    bool couldJoin() const {return joined;}
    Op type() const override { return Opcode::JOIN_GAME; }

private:
    bool joined;
    err_code exit_status;
};

#endif //JOINGAME_H
