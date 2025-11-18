#ifndef TPFINALNEEDFORSPEED_SRV_CURRENT_INFO_H
#define TPFINALNEEDFORSPEED_SRV_CURRENT_INFO_H

#include "server_msg.h"
#include "../../common_src/constants.h"

class SrvCurrentInfo: public SrvMsg {
public:
    SrvCurrentInfo(ID nextId, float cx, float cy, float dirX, float dirY,
                                   float rct, std::uint8_t rn):
            nextCheckpointId(nextId),
            checkX(cx),
            checkY(cy),
            hintDirX(dirX),
            hintDirY(dirY),
            raceTimeSeconds(rct),
            raceNumber(rn) {}

    Op type() const override { return Opcode::CURRENT_INFO; }

    float getSpeed() const { return this->speed; }
    float getRaceTimeSeconds() const { return this->raceTimeSeconds; }
    std::uint8_t getRaceNumber() const { return this->raceNumber; }

    ID getNextCheckpointId() const { return nextCheckpointId; }
    float getHintDirX() const { return hintDirX; }
    float getHintDirY() const { return hintDirY; }
    float getCheckX() const { return checkX; }
    float getCheckY() const { return checkY; }

private:
    float speed;
    float raceTimeSeconds;
    std::uint8_t raceNumber;

    ID nextCheckpointId = 0;
    float checkX = 0;
    float checkY = 0;
    float hintDirX = 0.f;
    float hintDirY = 0.f;
};

#endif  // TPFINALNEEDFORSPEED_SRV_CURRENT_INFO_H
