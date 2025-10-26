#pragma once
#include "client_protocol.h"
#include "client_types.h"

class ClientSender final: public Thread{
    public:

    explicit ClientSender(Socket& peer_sock);

    bool is_listening() const;

    protected:
    
    void run() override;

    private:

    bool parseLine(const std::string& line, std::string& cmd, std::string& param);

    ClientProtocol protocol;
    bool listening{true};
};
