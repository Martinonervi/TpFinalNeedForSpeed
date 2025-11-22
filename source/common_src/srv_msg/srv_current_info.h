#ifndef TPFINALNEEDFORSPEED_SRV_CURRENT_INFO_H
#define TPFINALNEEDFORSPEED_SRV_CURRENT_INFO_H

#include "server_msg.h"
#include "../../common_src/constants.h"

class SrvCurrentInfo: public SrvMsg {
public:
    SrvCurrentInfo(ID nextId, float checkX, float checkY, float angleHint, float distToCheck,
                                   float raceTime, std::uint8_t raceNumber, float speed
                   , uint8_t totalRaces):
            nextCheckpointId(nextId),
            checkX(checkX),
            checkY(checkY),
            angleHint(angleHint),
            distanceToChekpoint(distToCheck),
            raceTimeSeconds(raceTime),
            raceNumber(raceNumber),
            speed(speed),
            totalRaces(totalRaces) {}

    Op type() const override { return Opcode::CURRENT_INFO; }

    float getSpeed() const { return this->speed; }
    float getRaceTimeSeconds() const { return this->raceTimeSeconds; }
    std::uint8_t getRaceNumber() const { return this->raceNumber; }

    ID getNextCheckpointId() const { return nextCheckpointId; }
    float getCheckX() const { return checkX; }
    float getCheckY() const { return checkY; }
    float getAngleHint() const { return angleHint; }
    float getDistanceToCheckpoint() const { return distanceToChekpoint; }
    uint8_t getTotalRaces() const { return totalRaces; }

private:
    float speed;
    float raceTimeSeconds;
    std::uint8_t raceNumber;

    ID nextCheckpointId;
    float checkX;
    float checkY;
    float angleHint;
    float distanceToChekpoint;

    uint8_t totalRaces;

};

#endif  // TPFINALNEEDFORSPEED_SRV_CURRENT_INFO_H
