#ifndef TPFINALNEEDFORSPEED_MOVE_INFO_H
#define TPFINALNEEDFORSPEED_MOVE_INFO_H

#include <cstdint>

#include "client_msg.h"


class MoveMsg : public CliMsg {

public:
    MoveMsg() noexcept : accelerate(0), brake(0), steer(0), nitro(0) {}
    MoveMsg(uint8_t a, uint8_t b, int8_t s, uint8_t n)
            : accelerate(a), brake(b), steer(s), nitro(n) {}

    Op type() const override { return Opcode::Movement; };
    uint8_t getAccelerate() const { return this->accelerate; };
    uint8_t getBrake() const { return this->brake; };
    int8_t getSteer() const { return this->steer; };
    uint8_t getNitro() const { return this->nitro; };
    //void serialize(ClientProtocol& p) const override;


private:
    uint8_t accelerate; // acelerar, 0 false, 1 adelante, 2 reversa
    uint8_t brake; // frenar, 0 false, 1 true
    int8_t  steer; // [-1,1]
    uint8_t nitro; // [0,1]

};

#endif
