#ifndef TPFINALNEEDFORSPEED_SEND_PLAYER_H
#define TPFINALNEEDFORSPEED_SEND_PLAYER_H

#include "player.h"

class SendPlayer  : public Player  {

public:
    SendPlayer(ID id, CarType carType, float x, float y, float angleRad) noexcept
            : Player(id, carType, x, y, angleRad) {}
    Op type() const override { return Opcode::INIT_PLAYER; };

};


#endif
