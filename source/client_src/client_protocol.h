#pragma once

#include "client_types.h"
#include <cstring>

class ClientProtocol {
public:
    explicit ClientProtocol(Socket& peer);

    // manda por el socket el opcode para pedir nitro
    int requestNitro() const;

    // recibe y devuelve el struct de msg
    SrvMsg recvMsg();

    // lee (y por ahora descarta) el primer byte
    Op readActionByte() const;

    int sendCliMsg(const CliMsg& cliMsg) const;

private:
    Socket& peer;
};
