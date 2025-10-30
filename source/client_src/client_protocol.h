#pragma once
#include "client_types.h"
#include <cstring>

class ClientProtocol {
public:
    explicit ClientProtocol(Socket& peer);


    // recibe y devuelve el struct de msg
    SrvMsg recvSrvMsg();

    // lee (y por ahora descarta) el primer byte
    Op readActionByte() const;

    int sendCliMsg(const CliMsg& cliMsg) const;

private:
    Socket& peer;
};
