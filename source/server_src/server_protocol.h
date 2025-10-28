#pragma once

#include <memory>

#include "server_types.h"

class ServerProtocol {
public:
    explicit ServerProtocol(Socket& peer);

    // serializa y envía el mensaje por el socket
    int sendOutMsg(const constants::CliMsg& msg);

    // recibe mensaje y devuelve el opcode del mensaje recibido
    constants::Opcode recvMsg();

    constants::MoveInfo recvMoveInfo();

private:
    Socket& peer;
};
