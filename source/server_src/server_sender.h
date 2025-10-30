#pragma once
#include <memory>
#include "../common_src/constants.h"
#include "../common_src/socket.h"
#include "../common_src/thread.h"
#include "server_client_registry.h"
#include "server_protocol.h"
#include "server_types.h"

class Sender: public Thread {

public:
    Sender(Socket& peer_socket, SendQPtr queue);
    void stop() override;

    // señal: sender desconectado
    bool is_listening() const { return listening; }

protected:
    void run() override;

private:
    Socket& peer;
    SendQPtr msg_queue;
    ServerProtocol protocol;
    bool listening{true};
};
