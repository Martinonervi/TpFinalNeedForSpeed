//
// Created by Felipe Fialayre on 03/11/2025.
//

#ifndef JOINGAME_H
#define JOINGAME_H
#include "server_msg.h"

class JoinGame: public SrvMsg {
public:
    JoinGame(ID game_id): game_id(game_id){};

    ID const getGameID(){return game_id;}
    Op type() const override { return Opcode::JOIN_GAME; }

private:
    ID game_id;
};

#endif //JOINGAME_H
