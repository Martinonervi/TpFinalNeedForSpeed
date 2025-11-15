#ifndef TPFINALNEEDFORSPEED_INIT_PLAYER_H
#define TPFINALNEEDFORSPEED_INIT_PLAYER_H

#include <cstdint>
#include <string>

#include "../constants.h"

#include "client_msg.h"

class InitPlayer : public CliMsg {

public:

    InitPlayer(std::string name, CarType carType)
            : name(name), carType(carType) {}

    Op type() const override { return Opcode::INIT_PLAYER; }
    std::string getName() const { return this->name; }
    CarType getCarType() const { return this->carType; }


private:
    const std::string name;
    const CarType carType;


};


#endif