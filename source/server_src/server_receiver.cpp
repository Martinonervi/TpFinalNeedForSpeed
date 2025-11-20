#include "server_receiver.h"

#include <memory>
#include <utility>

#include "../common_src/cli_msg/disconnect_request.h"
#include "../common_src/cli_msg/init_player.h"
#include "../common_src/cli_msg/requeststats.h"
#include "../common_src/srv_msg/new_player.h"

Receiver::Receiver(Socket& peer_socket, ID clientID, GameManager& game_manager_ref, SendQPtr sender_queue):
        peer(peer_socket),
        id(clientID),
        protocol(peer),
        game_manager(game_manager_ref),
        sender_queue(std::move(sender_queue)){}


void Receiver::run() {
    try {
        Opcode op{};
        while (should_keep_running()) {
            try {
                op = protocol.recvOpcode();
            } catch (...) {
                peerClosed = true;
                break;
            }
            switch (op) {
                case Opcode::UPGRADE_REQUEST:{

                    //ACA ME BUGIE TINO completalo vos

                    break;
                }
                case Opcode::CLIENT_DISCONNECT: {
                    DisconnectReq req = protocol.recvDisconnectReq();
                    if (joined_game_id != 0) {
                        game_manager.LeaveGame(id, joined_game_id);
                        cmdQueue = nullptr;
                        joined_game_id = 0;
                        //auto msg = std::static_pointer_cast<SrvMsg>(
                        //    std::make_shared<ClientDisconnect>(id));
                        //sender_queue->push(msg);
                    }
                    break;
                }
                case Opcode::STATS:{
                    CliMsgPtr base = std::static_pointer_cast<CliMsg>(
                            std::make_shared<RequestStats>());
                    cmdQueue->push(Cmd{id, base});
                    break;
                }
                case Opcode::REQUEST_GAMES: {
                    MetadataGames games = game_manager.getGames();
                    auto msg = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<MetadataGames>(std::move(games)));
                    sender_queue->push(msg);
                    break;
                }
                case Opcode::JOIN_GAME:{
                    try {
                        RequestGame game_info = protocol.recvGameInfo();
                        auto [queue, game_id] = game_manager.CreateJoinGame(game_info.getGameID(), sender_queue, id);
                        cmdQueue = queue;
                        joined_game_id = game_id;
                    }catch (const std::exception& e) {
                        std::cerr << "[Receiver] error: " << e.what() << "\n";
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
                    std::cout << "cmd desconocido: " << static_cast<int>(op) << "\n";
                }
            }

        }
    }catch (const std::exception& e) {
        std::cerr << "[Receiver] uncaught: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[Receiver] uncaught: unknown\n";
    }

    listening = false;  // hilo terminó
}



void Receiver::stop() {
    Thread::stop();
    try { game_manager.LeaveGame(id, joined_game_id); } catch (...) {}
    try { peer.shutdown(0); } catch (...) {}
}
