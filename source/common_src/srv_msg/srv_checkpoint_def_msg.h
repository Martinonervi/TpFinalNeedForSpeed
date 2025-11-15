#ifndef TPFINALNEEDFORSPEED_SRV_CHECKPOINT_DEF_MSG_H
#define TPFINALNEEDFORSPEED_SRV_CHECKPOINT_DEF_MSG_H

#include "../constants.h"

#include "server_msg.h"

class SrvCheckpointDefMsg: public SrvMsg {
public:

    SrvCheckpointDefMsg(int checkpointId, float x1, float y1, float x2, float y2) noexcept :
            checkpointId(checkpointId), x1(x1), y1(y1), x2(x2), y2(y2) {};

    Op type() const override { return Opcode::CHECKPOINT_DEF; }

    uint16_t getCheckpointId() const { return this->checkpointId; }
    float getX1() const { return x1; }
    float getY1() const { return y1; }
    float getX2() const { return x2; }
    float getY2() const { return y2; }


private:
    ID checkpointId;
    float x1, y1, x2, y2;
};

#endif
