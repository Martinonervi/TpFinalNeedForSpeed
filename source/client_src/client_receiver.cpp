#include "client_receiver.h"

#include "../common_src/queue.h"
#include "../common_src/srv_msg/client_disconnect.h"
#include "../common_src/srv_msg/srv_car_hit_msg.h"
#include "../common_src/srv_msg/srv_checkpoint_hit_msg.h"
#include "../common_src/srv_msg/srv_current_info.h"
#include "../common_src/srv_msg/playerstats.h"

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
            case Opcode::Movement: {
                PlayerState ps = protocol.recvSrvMsg();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<PlayerState>(std::move(ps)));
                //std::cout << "[client Receiver] movement, id:" << ps.getPlayerId() << "\n";
                receiverQueue.push(base);
                break;
            }
            case Opcode::INIT_PLAYER: {
                SendPlayer sp = protocol.recvSendPlayer();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<SendPlayer>(std::move(sp)));
                std::cout << "[client Receiver] initPlayer, id:" << sp.getPlayerId() << "\n";
                receiverQueue.push(base);

                break;
            } case Opcode::NEW_PLAYER: {
                NewPlayer sp = protocol.recvNewPlayer();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<NewPlayer>(std::move(sp)));
                std::cout << "[client Receiver] newPlayer, id:" << sp.getPlayerId() << "\n";
                receiverQueue.push(base);
                break;
            }
            case Opcode::COLLISION: {
                SrvCarHitMsg msg = protocol.recvCollisionEvent();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<SrvCarHitMsg>(std::move(msg)));
                receiverQueue.push(base);
                break;
            }
            case Opcode::CHECKPOINT_HIT: {
                SrvCheckpointHitMsg msg = protocol.recvCheckpointHitEvent();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<SrvCheckpointHitMsg>(std::move(msg)));
                std::cout << "[client Receiver] CHECKPOINT_HIT, id:" << msg.getCheckpointId()
                          << "\n";
                receiverQueue.push(base);
                break;
            }
            case Opcode::CLIENT_DISCONNECT: {
                ClientDisconnect msg = protocol.recvClientDisconnect();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<ClientDisconnect>(std::move(msg)));
                std::cout << "[client Receiver] CLIENT_DISCONNECT, id:"
                << msg.getPlayerId()
                << "\n";
                receiverQueue.push(base);
                break;
            }
            case Opcode::CURRENT_INFO: {
                SrvCurrentInfo msg = protocol.recvCurrentInfo();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<SrvCurrentInfo>(std::move(msg)));
                receiverQueue.push(base);
                break;
            }
            case Opcode::STATS: {
                PlayerStats msg = protocol.recvStats();
                SrvMsgPtr base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<PlayerStats>(std::move(msg)));
                receiverQueue.push(base);
                std::cout << "[client Receiver] STATAS, ranking: "
                          << static_cast<int>(msg.getRacePosition())
                          << " y tiempo: " << msg.getTimeSecToComplete() << "\n";
                break;
            }

            default: {
                std::cout << "comando desconocido: " << op << "\n";
            }
        }
    }
}

bool ClientReceiver::is_listening() const { return !peerClosed; }


