#ifndef TPFINALNEEDFORSPEED_SRV_SEND_UPGRADE_H
#define TPFINALNEEDFORSPEED_SRV_SEND_UPGRADE_H

#include "server_msg.h"
#include "../../common_src/constants.h"

class SendUpgrade: public SrvMsg {
public:
    SendUpgrade(Upgrade& upgrade): upgrade(upgrade) {};
    Op type() const override { return Opcode::UPGRADE_SEND; }
    Upgrade getUpgrade() const { return upgrade; }
private:
    Upgrade& upgrade;
};

#endif
