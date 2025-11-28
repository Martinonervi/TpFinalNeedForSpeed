#include "client_sender.h"

#include <sstream>

#include "../common_src/cli_msg/requestgame.h"
#include "../common_src/cli_msg/cli_start_game.h"

ClientSender::ClientSender(ClientProtocol& protocol, Queue<CliMsgPtr>& senderQueue)
    :protocol(protocol), senderQueue(senderQueue){}

void ClientSender::run(){
    while(should_keep_running()) {
        try{
            CliMsgPtr cliMsg = senderQueue.pop();
            switch (cliMsg->type()) {
                case (Opcode::UPGRADE_REQUEST): {
                    protocol.sendRequestUpgrade(dynamic_cast<RequestUpgrade&>(*cliMsg));
                    break;
                }
                case (Opcode::JOIN_GAME): { // sacar, ya moví la lógica de lugar
                    protocol.sendRequestGame(dynamic_cast<RequestGame&>(*cliMsg));
                    break;
                }
                case (Opcode::Movement): {
                    protocol.sendClientMove(dynamic_cast<const MoveMsg&>(*cliMsg));
                    break;
                }
                case (Opcode::INIT_PLAYER): {
                    protocol.sendInitPlayer(dynamic_cast<const InitPlayer&>(*cliMsg));
                    break;
                }
                case (Opcode::START_GAME): {
                    protocol.sendStartGame(dynamic_cast<const StartGame&>(*cliMsg));
                    break;
                }
                case (Opcode::REQUEST_CHEAT): {
                    protocol.sendCheat(dynamic_cast<const CheatRequest&>(*cliMsg));
                    break;
                }

                default: {
                    std::cout << "comando desconocido: " << cliMsg->type() << "\n";
                }
            }
        } catch (const std::out_of_range& e) {
            std::cout << "[Client Sender]cmd desconocido: " << "\n";
        } catch (const std::exception& e) {
            std::cerr << "client_main error: " << e.what() << "\n";
        }
    }
}
