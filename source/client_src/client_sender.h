#pragma once
#include "client_protocol.h"
#include "client_types.h"

class ClientSender: public Thread {
public:
    explicit ClientSender(Socket& peer_sock, Queue<CliMsg>& senderQueue);
    bool is_listening() const;
    void run() override;

private:
    ClientProtocol protocol;
    Queue<CliMsg>& senderQueue;
    bool listening{true};
};
