#include "client_receiver.h"
#include "../common_src/queue.h"

ClientReceiver::ClientReceiver(Socket& peer_sock, Queue<SrvMsg>& receiverQueue)
    :protocol(peer_sock), receiverQueue(receiverQueue){}


void ClientReceiver::run(){
    while (should_keep_running()){
        SrvMsg msg = protocol.recvSrvMsg();
        switch (msg.type) {
            case (Opcode::Movement): {
                std::cout << "llegue al client Receiver, id:" << msg.posicion.player_id << "\n";
                receiverQueue.push(msg);
                break;
            }
            default: {
                std::cout << "cmd desconocido: " << msg.type << "\n";
            }
        }
    }
}

