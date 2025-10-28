#pragma once

#include "client_types.h"
#include <cstring>

class ClientProtocol {
public:
    explicit ClientProtocol(Socket& peer);

    // manda por el socket el opcode para pedir nitro
    int requestNitro();

    // recibe y devuelve el struct de msg
    constants::SrvMsg recvMsg();

    // lee (y por ahora descarta) el primer byte
    constants::Op readActionByte();

    int sendCliMsg(const constants::CliMsg& cliMsg);

private:
    Socket& peer;
};
