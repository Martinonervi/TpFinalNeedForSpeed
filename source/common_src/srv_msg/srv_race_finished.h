#ifndef TPFINALNEEDFORSPEED_SRV_RACE_FINISHED_H
#define TPFINALNEEDFORSPEED_SRV_RACE_FINISHED_H

#include "server_msg.h"
#include "../constants.h"

class SrvRaceFinished : public SrvMsg {
public:
    SrvRaceFinished() noexcept = default;

    Op type() const override { return Opcode::RACE_FINISHED; }
};

#endif