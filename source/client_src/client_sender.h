#pragma once
#include "client_protocol.h"
#include "client_types.h"

class ClientSender: public Thread {
public:
    explicit ClientSender(Socket& peer_sock, Queue<CliMsgPtr>& senderQueue);
    bool is_listening() const;
    void run() override;
    bool leerStdinYEncolar();
    bool parseLine(const std::string& line, std::string& cmd, std::string& param);

private:
    ClientProtocol protocol;
    Queue<CliMsgPtr>& senderQueue;
    bool listening{true};
};
