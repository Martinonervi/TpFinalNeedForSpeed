#include "client_receiver.h"
#include "../common_src/queue.h"

ClientReceiver::ClientReceiver(ClientProtocol& protocol, Queue<SrvMsgPtr>& receiverQueue)
    :protocol(protocol), receiverQueue(receiverQueue){}

void ClientReceiver::run(){
    while (should_keep_running()){
        Op op;
        try {
            op = protocol.readActionByte();
        } catch (...) {
            peerClosed = true;
            break;
        }

        switch (op) {
            case (Opcode::Movement): {
                PlayerState ps = protocol.recvSrvMsg();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<PlayerState>(std::move(ps)));
                std::cout << "[client Receiver] movement, id:" << ps.getPlayerId() << "\n";
                receiverQueue.push(base);
                break;
            }
            case (Opcode::INIT_PLAYER): {
                SendPlayer sp = protocol.recvSendPlayer();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<SendPlayer>(std::move(sp)));
                std::cout << "[client Receiver] initPlayer, id:" << sp.getPlayerId() << "\n";
                receiverQueue.push(base);

                break;
            } case (Opcode::NEW_PLAYER): {
                // fran: fijate que no sea el id del client, o q no sea un repetido
                NewPlayer sp = protocol.recvNewPlayer();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<NewPlayer>(std::move(sp)));
                std::cout << "[client Receiver] newPlayer, id:" << sp.getPlayerId() << "\n";
                receiverQueue.push(base);
                break;
            }
            default: {
                std::cout << "comando desconocido: " << op << "\n";
            }
        }
    }
}

bool ClientReceiver::is_listening() const { return !peerClosed; }


