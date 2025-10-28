#pragma once
#include "client_protocol.h"
#include "client_types.h"

class ClientReceiver: public Thread{
    public:

    ClientReceiver(Socket& peer_socket, Queue<constants::SrvMsg>& receiverQueue);

    protected:

    void run() override;

    private:

    ClientProtocol protocol;
    Queue<constants::SrvMsg>& receiverQueue;
    Printer printer;
};
