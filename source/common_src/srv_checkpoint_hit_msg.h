#ifndef TPFINALNEEDFORSPEED_SRV_CAR_PASS_CHECKPOINT_MSG_H
#define TPFINALNEEDFORSPEED_SRV_CAR_PASS_CHECKPOINT_MSG_H

#include "server_msg.h"
#include "constants.h"

class SrvCheckpointHitMsg: public SrvMsg {
public:

    SrvCheckpointHitMsg(ID id, ID checkpoint_id) noexcept
            : player_id(id), checkpoint_id(checkpoint_id) {}

    Op type() const override { return Opcode::CHECKPOINT_HIT; }

    ID getPlayerId() const { return this->player_id; }
    ID getCheckpointId() const { return this->checkpoint_id; }


private:
    ID player_id;
    ID checkpoint_id;
};

#endif
