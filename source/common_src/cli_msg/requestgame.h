
#ifndef JOIN_GAME_H
#define JOIN_GAME_H
#include "client_msg.h"

class RequestGame: public CliMsg {
public:
    RequestGame(ID game_id): game_id(game_id){};
    Op type() const override { return Opcode::JOIN_GAME; }
    ID const getGameID(){return game_id;}
private:
    ID game_id;
};

#endif //JOIN_GAME_H
