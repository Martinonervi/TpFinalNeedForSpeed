#ifndef TPFINALNEEDFORSPEED_SRV_SEND_UPGRADE_H
#define TPFINALNEEDFORSPEED_SRV_SEND_UPGRADE_H

#include "server_msg.h"
#include "../../common_src/constants.h"

class SendUpgrade: public SrvMsg {
public:
    SendUpgrade(Upgrade upgrade, bool success): upgrade(upgrade), success(success) {};
    Op type() const override { return Opcode::UPGRADE_SEND; }
    Upgrade getUpgrade() const { return upgrade; }
    bool couldBuy() const {return success;}
private:
    Upgrade upgrade;
    bool success;
};

#endif
