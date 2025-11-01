#ifndef TPFINALNEEDFORSPEED_SEND_PLAYER_H
#define TPFINALNEEDFORSPEED_SEND_PLAYER_H

#include "server_msg.h"
#include <cstdint>
#include "constants.h"

class SendPlayer: public SrvMsg {
public:
    SendPlayer(ID id, CarType carType, float x, float y, float angleRad) noexcept
            : player_id(id), carType(carType), x(x), y(y), angleRad(angleRad) {}

    Op type() const override { return Opcode::INIT_PLAYER; }

    uint16_t getPlayerId() const { return this->player_id; }
    CarType getCarType() const { return this->carType; }
    float    getX()        const { return this->x; }
    float    getY()        const { return y; }
    float    getAngleRad() const { return angleRad; }


private:
    ID player_id;
    CarType carType;
    float x;
    float y;
    float angleRad;

};

#endif
