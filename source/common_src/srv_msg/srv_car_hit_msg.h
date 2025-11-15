#ifndef TPFINALNEEDFORSPEED_SRVCARHITMSG_H
#define TPFINALNEEDFORSPEED_SRVCARHITMSG_H

#include "../constants.h"

#include "server_msg.h"

class SrvCarHitMsg: public SrvMsg {
public:

    SrvCarHitMsg(ID id, float health) noexcept
            : player_id(id), health(health) {}

    Op type() const override { return Opcode::COLLISION; }

    uint16_t getPlayerId() const { return this->player_id; }
    float getCarHealth() const { return this->health; }


private:
    ID player_id;
    float health;
};

#endif
