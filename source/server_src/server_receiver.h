#pragma once

#include <memory>

#include "../common_src/constants.h"
#include "../common_src/socket.h"
#include "../common_src/thread.h"

#include "server_client_registry.h"
#include "server_protocol.h"
#include "server_types.h"

class Receiver: public Thread {
    using gameLoopQueue = serv_types::gameLoopQueue;

public:
    Receiver(Socket& peerSocket, gameLoopQueue& queue, serv_types::ID clientId);
    void stop() override;

    // señal: sender desconectado
    bool is_listening() const { return listening; }
    // señal: cliente desconectado (señal disparadora)
    bool is_peer_closed() const { return peerClosed; }

protected:
    void run() override;  // bucle de lectura

private:
    Socket& peer;
    gameLoopQueue& cmdQueue;
    serv_types::ID id{0};
    ServerProtocol protocol;

    bool listening{true};
    bool peerClosed{false};
};
