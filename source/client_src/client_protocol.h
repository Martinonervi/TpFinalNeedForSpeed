#pragma once

#include "client_types.h"
#include <cstring>

class ClientProtocol {
public:
    explicit ClientProtocol(Socket& peer);

    // manda por el socket el opcode para pedir nitro
    int requestNitro() const;

    // recibe y devuelve el struct de msg
    constants::SrvMsg recvMsg();

    // lee (y por ahora descarta) el primer byte
    constants::Op readActionByte() const;

    int sendCliMsg(const constants::CliMsg& cliMsg) const;

private:
    Socket& peer;
};
