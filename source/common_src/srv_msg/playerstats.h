#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H

#include "server_msg.h"

class PlayerStats: public SrvMsg {
public:
    PlayerStats(uint8_t racePosition, float timeSecToComplete):
            racePosition(racePosition),
            timeSecToComplete(timeSecToComplete){};
    Op type() const override { return Opcode::STATS; }
    uint8_t getRacePosition() const { return racePosition;}
    float getTimeSecToComplete() const { return timeSecToComplete; }

private:
    uint8_t racePosition;
    float timeSecToComplete;
};

#endif //PLAYERSTATS_H
