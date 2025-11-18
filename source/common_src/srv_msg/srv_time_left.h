//
// Created by Felipe Fialayre on 18/11/2025.
//

#ifndef SRV_TIME_LEFT_H
#define SRV_TIME_LEFT_H
#include "../constants.h"

class TimeLeft: public SrvMsg {
public:
    TimeLeft(uint8_t timeLeft):timeLeft(timeLeft){};
    Op type() const override { return Opcode::TIME; }
    uint8_t getTimeLeft() const { return timeLeft; }

private:
    uint8_t timeLeft;
};
#endif //SRV_TIME_LEFT_H
