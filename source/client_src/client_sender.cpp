#include "client_sender.h"

#include <sstream>

#include "../common_src/cli_msg/requestgame.h"
#include "../common_src/cli_msg/cli_start_game.h"

ClientSender::ClientSender(ClientProtocol& protocol, Queue<CliMsgPtr>& senderQueue)
    :protocol(protocol), senderQueue(senderQueue){}

void ClientSender::run(){
    while(should_keep_running() /*and leerStdinYEncolar()*/) {
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
    listening = false;
}

//esta funcion esta muy util para probar de stdin el protocolo
bool ClientSender::leerStdinYEncolar() {
    std::string line, cmd, param;


    if (!std::getline(std::cin, line)) {
        return false;
    }
    if (!parseLine(line, cmd, param)){
        return false;
    }
    if (line.empty()) {
        return true;
    }


    // input -> solo para testear que se mande bien los mensajes
/*
    uint8_t accel = 1;
    uint8_t brake = 1;
    int8_t  steer = 1;
    uint8_t nitro = 1;

    auto move = std::make_shared<MoveMsg>(accel, brake, steer, nitro);
    CliMsgPtr base = move; //ni hace falta casteo
*/

    std::shared_ptr<InitPlayer> ip = std::make_shared<InitPlayer>("pancho",CarType::CAR_GREEN);
    CliMsgPtr base = ip;
    senderQueue.push(base);
    std::cout << "mandando el cmd por la senderQueue de client\n";
    return true;

}

bool ClientSender::parseLine(const std::string& line, std::string& cmd, std::string& param) {
    cmd.clear();  // vaciamos los buffers por las dudas
    param.clear();

    std::istringstream iss(line);
    iss >> cmd;

    if (cmd == EXIT_CMD)
        return false;  // salir del loop si el cliente pone exit
    iss >> param;      // si no encuentro nada no hace nada (param vacio)

    return true;
}

bool ClientSender::is_listening() const { return listening; }