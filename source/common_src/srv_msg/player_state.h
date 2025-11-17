#ifndef TPFINALNEEDFORSPEED_PLAYER_STATE_H
#define TPFINALNEEDFORSPEED_PLAYER_STATE_H
#include <cstdint>

#include "../constants.h"

#include "server_msg.h"

class PlayerState: public SrvMsg {
public:

    PlayerState(ID id, float x, float y, float ang) noexcept
            : player_id(id), x(x), y(y), angleRad(ang) {}

    Op type() const override { return Opcode::Movement; }

    uint16_t getPlayerId() const { return this->player_id; }
    float    getX()        const { return this->x; }
    float    getY()        const { return y; }
    float    getAngleRad() const { return angleRad; }

    ID getNextCheckpointId() const { return nextCheckpointId; }
    float getHintDirX() const { return hintDirX; }
    float getHintDirY() const { return hintDirY; }
    float getCheckX() const { return checkX; }
    float getCheckY() const { return checkY; }

    void setCheckpointInfo(ID nextId, float cx, float cy, float dirX, float dirY) {
        nextCheckpointId = nextId;
        checkX = cx;
        checkY = cy;
        hintDirX = dirX;
        hintDirY = dirY;
    }
private:
    ID player_id;
    float    x;
    float    y;
    float    angleRad;

    ID nextCheckpointId = 0;
    float checkX = 0;
    float checkY = 0;
    float hintDirX = 0.f;
    float hintDirY = 0.f;
};


#endif
