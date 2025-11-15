#ifndef TPFINALNEEDFORSPEED_CLIENT_MSG_H
#define TPFINALNEEDFORSPEED_CLIENT_MSG_H
#include "../opcodes.h"

class CliMsg {
public:
    virtual ~CliMsg()= default;
    virtual Op type() const= 0; //abstracto puro ya q ni la imlementa
    //virtual void serialize(ClientProtocol& p) const;


protected:
    CliMsg() noexcept = default;
};

#endif


