#pragma once
#include "client_types.h"
#include <cstring>

class ClientProtocol {
public:
    explicit ClientProtocol(Socket& peer);


    // recibe y devuelve el struct de msg
    PlayerState recvSrvMsg();

    // lee (y por ahora descarta) el primer byte
    Op readActionByte() const;
    int sendClientMove(const MoveMsg& moveMsg) const;

private:
    Socket& peer;
};
