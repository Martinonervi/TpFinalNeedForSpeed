#include "server_sender.h"

#include <memory>
#include <utility>
#include <vector>

#include "../common_src/srv_msg/client_disconnect.h"
#include "../common_src/srv_msg/send_player.h"
#include "../common_src/srv_msg/srv_car_hit_msg.h"
#include "../common_src/srv_msg/srv_current_info.h"
#include "../common_src/srv_msg/srv_upgrade_logic.h"
#include "../common_src/srv_msg/srv_recommended_path.h"

Sender::Sender(Socket& peer_socket, SendQPtr queue):
        peer(peer_socket), msg_queue(std::move(queue)), protocol(peer) {}

void Sender::stop() {
    Thread::stop();
    if (msg_queue)
        msg_queue->close();
    try {
        peer.shutdown(1);
    } catch (...) {}
}
void Sender::run() {
    try {
        while (should_keep_running()) {
            if (!msg_queue) break;
            SrvMsgPtr msg = msg_queue->pop();

            int n;

            switch (msg->type()) {
                case Opcode::TIME: {
                    if (!playing){continue;}
                    n = protocol.sendTimeLeft(dynamic_cast<TimeLeft&>(*msg));
                    break;
                }
                case Opcode::STATS:{ // no esta testeado
                    if (!playing){continue;}
                    n = protocol.sendPlayerStats(dynamic_cast<PlayerStats&>(*msg));
                    break;
                }
                case Opcode::REQUEST_GAMES:{
                    n = protocol.sendGames(dynamic_cast<const MetadataGames&>(*msg));
                    break;
                }
                case Opcode::JOIN_GAME:{
                    n = protocol.sendGameInfo(dynamic_cast<const JoinGame&>(*msg));
                    break;
                }
                case Opcode::Movement: {
                    if (!playing){continue;}
                    n = protocol.sendPlayerState(dynamic_cast<const PlayerState&>(*msg));
                    break;
                }
                case Opcode::INIT_PLAYER:
                case Opcode::NEW_PLAYER:
                {
                    playing = true;
                    n = protocol.sendPlayerInit(dynamic_cast<Player&>(*msg));
                    break;
                }
                case Opcode::COLLISION: {
                    if (!playing){continue;}
                    n = protocol.sendCollisionEvent(dynamic_cast<SrvCarHitMsg&>(*msg));
                    break;
                }
                case Opcode::CHECKPOINT_HIT: {
                    if (!playing){continue;}
                    n = protocol.sendCheckpointHit(dynamic_cast<SrvCheckpointHitMsg&>(*msg));
                    break;
                }
                case Opcode::CLIENT_DISCONNECT: {
                    if (!playing){continue;}
                    n = protocol.sendClientDisconnect(dynamic_cast<ClientDisconnect&>(*msg));
                    break;
                }
                case Opcode::CURRENT_INFO: {
                    if (!playing){continue;}
                    n = protocol.sendCurrentInfo(dynamic_cast<SrvCurrentInfo&>(*msg));
                    break;
                }
                case Opcode::UPGRADE_SEND: {
                    if (!playing){continue;}
                    n = protocol.sendUpgrade(dynamic_cast<SendUpgrade&>(*msg));
                    break;
                }
                case Opcode::UPGRADE_LOGIC: {
                    if (!playing){continue;}
                    n = protocol.sendUpgradeLogic(dynamic_cast<UpgradeLogic&>(*msg));
                    break;
                }
                case Opcode::RECOMMENDED_PATH: {
                    if (!playing){continue;}
                    n = protocol.sendRecommendedPath(dynamic_cast<RecommendedPath&>(*msg));
                    break;
                }
                case Opcode::CAR_SELECT: {
                    n = protocol.sendCarConfirmation(dynamic_cast<CarSelect&>(*msg));
                    std::cout << "[Sender] mande opcode"<< std::endl;
                    break;
                }
                default: {
                    std::cout << "cmd desconocido: " << msg->type() << "\n";
                    n = 0; //quiero salir
                }
            }

            if (n == 0)
                break;
        }
    } catch (const ClosedQueue&) {
        // se cerró la cola mientras esperaba --> client handler lo interrumpió
    } catch (...) {
        // error sending probably
    }
    listening = false;
}


