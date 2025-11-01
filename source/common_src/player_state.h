#ifndef TPFINALNEEDFORSPEED_PLAYER_STATE_H
#define TPFINALNEEDFORSPEED_PLAYER_STATE_H
#include "server_msg.h"
#include <cstdint>

class PlayerState: public SrvMsg {
public:

    PlayerState(uint16_t id, float x, float y, float ang) noexcept
            : player_id(id), x(x), y(y), angleRad(ang) {}

    Op type() const override { return Opcode::Movement; }

    uint16_t getPlayerId() const { return this->player_id; }
    float    getX()        const { return this->x; }
    float    getY()        const { return y; }
    float    getAngleRad() const { return angleRad; }


private:
    uint16_t player_id;
    float    x;
    float    y;
    float    angleRad;
};


#endif
