#pragma once
#include "client_protocol.h"
#include "client_types.h"

class ClientReceiver: public Thread{
public:
    ClientReceiver(Socket& peer_socket, Queue<SrvMsg>& receiverQueue);

    void run() override;

private:
    ClientProtocol protocol;
    Queue<SrvMsg>& receiverQueue;
    Printer printer;
};
