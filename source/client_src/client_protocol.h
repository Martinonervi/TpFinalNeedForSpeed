#pragma once

#include "client_types.h"

class ClientProtocol {
public:
    explicit ClientProtocol(Socket& peer);

    // manda por el socket el opcode para pedir nitro
    int requestNitro();

    // recibe y devuelve el struct de msg
    constants::OutMsg recvMsg();

    // lee (y por ahora descarta) el primer byte
    constants::Op readActionByte();

private:
    Socket& peer;
};
