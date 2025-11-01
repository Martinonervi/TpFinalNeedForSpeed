#ifndef TPFINALNEEDFORSPEED_SERVER_MSG_H
#define TPFINALNEEDFORSPEED_SERVER_MSG_H

#include "opcodes.h"

class SrvMsg {
public:
    virtual ~SrvMsg() = default;
    virtual Op  type() const = 0;

protected:
    SrvMsg() noexcept = default;
};

#endif
