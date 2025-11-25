//
// Created by Felipe Fialayre on 24/11/2025.
//

#ifndef SRV_CAR_SELECT_H
#define SRV_CAR_SELECT_H

#include "../constants.h"

#include "server_msg.h"

class CarSelect: public SrvMsg {
public:

    CarSelect(bool selected) noexcept
            : selected(selected) {}

    Op type() const override { return Opcode::CAR_SELECT; }

    bool isSelected() const {return selected;}


private:
    bool selected;
};
#endif //SRV_CAR_SELECT_H
