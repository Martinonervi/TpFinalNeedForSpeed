//
// Created by Felipe Fialayre on 18/11/2025.
//

#ifndef SRV_TIME_LEFT_H
#define SRV_TIME_LEFT_H
#include "../constants.h"

class TimeLeft: public SrvMsg {
public:
    TimeLeft(uint8_t timeLeft, bool upgradesEnabled):
    timeLeft(timeLeft),
    upgradesEnabled(upgradesEnabled) {};

    Op type() const override { return Opcode::TIME; }

    uint8_t getTimeLeft() const { return timeLeft; }
    bool    getUpgradesEnabled() const { return upgradesEnabled; }

private:
    uint8_t timeLeft;
    bool    upgradesEnabled;
};
#endif //SRV_TIME_LEFT_H
