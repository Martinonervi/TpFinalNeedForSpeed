#include "server_receiver.h"
#include <memory>
#include <utility>
#include "../common_src/init_player.h"
#include "../common_src/new_player.h"

Receiver::Receiver(Socket& peer_socket, ID clientID, GameManager& game_manager_ref, SendQPtr sender_queue):
        peer(peer_socket),
        id(clientID),
        protocol(peer),
        game_manager(game_manager_ref),
        sender_queue(std::move(sender_queue)){}


void Receiver::run() {
    Opcode op{};
    while (should_keep_running()) {
        try {
            op = protocol.recvOpcode();
        } catch (...) {
            peerClosed = true;
            break;
        }
        switch (op) {
            case Opcode::JOIN_GAME:{
                try {
                    JoinGame game_info = protocol.getGameInfo();
                    auto [queue, game_id] = game_manager.CreateJoinGame(game_info.getGameID(), sender_queue, id);
                    cmdQueue = queue;
                    joined_game_id = game_id;
                }catch (const char* msg) {
                    //aca atrapo el error si la partida estaba llena
                }
                break;
            }
            case Opcode::Movement: {
                MoveMsg mv = protocol.recvMoveInfo();  // lo recibe por valor
                CliMsgPtr base = std::static_pointer_cast<CliMsg>(
                        std::make_shared<MoveMsg>(std::move(mv)));
                cmdQueue->push(Cmd{id, base});
                break;
            }
            case Opcode::INIT_PLAYER: {
                InitPlayer ip = protocol.recvInitPlayer();
                CliMsgPtr base = std::static_pointer_cast<CliMsg>(
                        std::make_shared<InitPlayer>(std::move(ip)));
                cmdQueue->push(Cmd{id, base});

                break;
                }
            default: {
                std::cout << "cmd desconocido: " << op << "\n";
            }
        }

    }
    listening = false;  // hilo terminó
}



void Receiver::stop() {
    Thread::stop();
    try { game_manager.LeaveGame(id, joined_game_id); } catch (...) {}
    try {
        peer.shutdown(0);
    } catch (...) {}
}
