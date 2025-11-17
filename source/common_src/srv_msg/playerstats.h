//
// Created by Felipe Fialayre on 17/11/2025.
//

#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H
#include "server_msg.h"

class PlayerStats: public SrvMsg {
public:
    PlayerStats(uint8_t cant_checkpoints): checkpoints(cant_checkpoints){} ;
    Op type() const override { return Opcode::STATS; }
    uint8_t getCheckpoints() const { return checkpoints;}
private:
    uint8_t checkpoints;
};

#endif //PLAYERSTATS_H
