#ifndef TPFINALNEEDFORSPEED_NEW_PLAYER_H
#define TPFINALNEEDFORSPEED_NEW_PLAYER_H

#include "player.h"

class NewPlayer  : public Player  {

public:
    NewPlayer(ID id, CarType carType, float x, float y, float angleRad) noexcept
            : Player(id, carType, x, y, angleRad) {}

    Op type() const override { return Opcode::NEW_PLAYER; };

};

#endif
