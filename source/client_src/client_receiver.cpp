#include "client_receiver.h"
#include "../common_src/queue.h"

ClientReceiver::ClientReceiver(Socket& peer_sock, Queue<SrvMsgPtr>& receiverQueue)
    :protocol(peer_sock), receiverQueue(receiverQueue){}


void ClientReceiver::run(){
    while (should_keep_running()){
        Op op = protocol.readActionByte();

        switch (op) {
            case (Opcode::Movement): {
                PlayerState msg = protocol.recvSrvMsg();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<PlayerState>(std::move(msg)));
                std::cout << "llegue al client Receiver, id:" << msg.getPlayerId() << "\n";
                receiverQueue.push(base);
                break;
            }
            default: {
                std::cout << "cmd desconocido: " << op << "\n";
            }
        }
    }
}

