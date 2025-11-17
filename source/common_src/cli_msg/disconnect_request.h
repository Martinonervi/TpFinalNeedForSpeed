//
// Created by Felipe Fialayre on 16/11/2025.
//

#ifndef DISCONNECT_H
#define DISCONNECT_H
#include "../constants.h"
#include "client_msg.h"

class DisconnectReq : public CliMsg {

public:
    DisconnectReq(ID game_id)
            : game_id(game_id) {}

    Op type() const override { return Opcode::CLIENT_DISCONNECT ; }
    ID getGameID() const { return game_id; }

private:
    const ID game_id;
};

#endif //DISCONNECT_H
