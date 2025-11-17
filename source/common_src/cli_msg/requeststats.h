//
// Created by Felipe Fialayre on 17/11/2025.
//

#include "client_msg.h"

#ifndef REQUESTSTAT_H
#define REQUESTSTAT_H

class RequestStats: public CliMsg {
public:
    RequestStats() = default;
    Op type() const override { return Opcode::STATS; }
};

#endif //REQUESTSTAT_H
