#include "client_sender.h"

#include <sstream>

ClientSender::ClientSender(Socket& peer_sock)
    :protocol(peer_sock){}


void ClientSender::run(){
    std::string line, cmd, param;
    while (std::getline(std::cin, line) && (parseLine(line, cmd, param) && should_keep_running())) {
        try{
            constants::Op op = client_types::cmdToOp.at(cmd);
            switch (op) {
                case (constants::ClientMSG): {
                    protocol.requestNitro();
                    break;
                }
                default: {
                    std::cout << "cmd desconocido: " << cmd << "\n"; 
                }
            } 
        } catch (const std::out_of_range& e) {
            std::cout << "cmd desconocido: " << cmd << "\n";
        } catch (const std::exception& e) {
            std::cerr << "client_main error: " << e.what() << "\n";
        }
        
    }
    listening = false;
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

bool ClientSender::is_listening() { return listening; }