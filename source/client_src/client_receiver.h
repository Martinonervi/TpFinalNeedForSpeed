#pragma once
#include "client_protocol.h"
#include "client_types.h"

class ClientReceiver: public Thread{
public:
    ClientReceiver(ClientProtocol& peer_socket, Queue<SrvMsgPtr>& receiverQueue);
    void run() override;
    /*bool is_listening() const;*/

private:
    ClientProtocol& protocol;
    Queue<SrvMsgPtr>& receiverQueue;
    Printer printer;
    bool peerClosed{false};
};
