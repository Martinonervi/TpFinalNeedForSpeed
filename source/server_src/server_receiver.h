#pragma once

#include <memory>

#include "../common_src/constants.h"
#include "../common_src/socket.h"
#include "../common_src/thread.h"

#include "server_client_registry.h"
#include "server_game_manager.h"
#include "server_protocol.h"
#include "server_types.h"

class Receiver: public Thread {

public:
    Receiver(Socket& peerSocket, ID clientId, GameManager& game_manager_ref, SendQPtr sendq);
    void stop() override;

    // señal: sender desconectado
    bool is_listening() const { return listening; }
    // señal: cliente desconectado (señal disparadora)
    bool is_peer_closed() const { return peerClosed; }

    void setCmdQueue(std::shared_ptr<gameLoopQueue>);

protected:
    void run() override;  // bucle de lectura

private:
    Socket& peer;
    std::shared_ptr<gameLoopQueue> cmdQueue;
    ID id{0};
    ServerProtocol protocol;
    GameManager& game_manager;
    SendQPtr sender_queue;
    ID joined_game_id;

    bool listening{true};
    bool peerClosed{false};
};
