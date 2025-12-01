//
// Created by Felipe Fialayre on 28/11/2025.
//

#include "server_msg.h"
#include "../../common_src/constants.h"

#ifndef SRV_DISCONNECTION_H
#define SRV_DISCONNECTION_H



class SrvDisconnection: public SrvMsg {
public:

    SrvDisconnection() = default;
    Op type() const override { return Opcode::SRV_DISCONNECTION; }
};


#endif //SRV_DISCONNECTION_H
