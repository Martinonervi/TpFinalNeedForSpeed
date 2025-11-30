#ifndef TPFINALNEEDFORSPEED_SRVCARHITMSG_H
#define TPFINALNEEDFORSPEED_SRVCARHITMSG_H

#include "../constants.h"

#include "server_msg.h"

class SrvCarHitMsg: public SrvMsg {
public:

    SrvCarHitMsg(ID id, float health, float totalHealth) noexcept
            : player_id(id), health(health), totalHealth(totalHealth) {}

    Op type() const override { return Opcode::COLLISION; }

    ID getPlayerId() const { return this->player_id; }
    float getCarHealth() const { return this->health; }
    float getTotalHealth() const { return this->totalHealth; }


private:
    ID player_id;
    float health;
    float totalHealth;
};

#endif
