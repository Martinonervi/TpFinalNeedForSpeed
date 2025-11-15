#ifndef TPFINALNEEDFORSPEED_CLIENT_DISCONNECT_H
#define TPFINALNEEDFORSPEED_CLIENT_DISCONNECT_H

#include "../constants.h"

#include "server_msg.h"

class ClientDisconnect: public SrvMsg {
public:
    explicit ClientDisconnect(ID id) noexcept
            : player_id(id) {}

    Op type() const override { return Opcode::CLIENT_DISCONNECT; }
    ID getPlayerId() const { return this->player_id; }

private:
    ID player_id;
};

#endif  // TPFINALNEEDFORSPEED_CLIENT_DISCONNECT_H
