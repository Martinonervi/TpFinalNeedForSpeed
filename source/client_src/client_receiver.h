#pragma once
#include "client_protocol.h"
#include "client_types.h"

class ClientReceiver: public Thread{
public:
    ClientReceiver(Socket& peer_socket, Queue<SrvMsgPtr>& receiverQueue);

    void run() override;

private:
    ClientProtocol protocol;
    Queue<SrvMsgPtr>& receiverQueue;
    Printer printer;
};
