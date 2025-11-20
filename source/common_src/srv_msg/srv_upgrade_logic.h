#ifndef TPFINALNEEDFORSPEED_SRV_UPGRADE_LOGIC_H
#define TPFINALNEEDFORSPEED_SRV_UPGRADE_LOGIC_H

#include "server_msg.h"
#include "../../common_src/constants.h"

class UpgradeLogic: public SrvMsg {
public:
    UpgradeLogic(std::vector<Upgrade>& upgrades): upgrades(upgrades) {};
    Op type() const override { return Opcode::UPGRADE_LOGIC; }
    std::vector<Upgrade> getUpgrades() const { return upgrades; }
private:
    std::vector<Upgrade>& upgrades;
};

#endif
