#ifndef TPFINALNEEDFORSPEED_SRV_REQUEST_UPGRADE_H
#define TPFINALNEEDFORSPEED_SRV_REQUEST_UPGRADE_H

#include "../constants.h"
#include "client_msg.h"

class RequestUpgrade : public CliMsg {

public:
    RequestUpgrade(Upgrade& upgrade): upgrade(upgrade) {};
    Op type() const override { return Opcode::UPGRADE_REQUEST; }
    Upgrade getUpgrade() const { return upgrade; }
private:
    Upgrade& upgrade;
    bool success;
};


#endif
