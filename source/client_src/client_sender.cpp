#include "client_sender.h"
#include <sstream>

ClientSender::ClientSender(Socket& peer_sock, Queue<CliMsg>& senderQueue)
    :protocol(peer_sock), senderQueue(senderQueue){}

void ClientSender::run(){
    //std::string line, cmd, param;
    //while (std::getline(std::cin, line) && (parseLine(line, cmd, param) && should_keep_running())) {

    while(should_keep_running() and leerStdinYEncolar()) {
        try{
            CliMsg cliMsg = senderQueue.pop();

            switch (cliMsg.event_type) {
                case (Opcode::Nitro): {
                    protocol.requestNitro();
                    break;
                }
                case (Opcode::Movement): {
                    protocol.sendCliMsg(cliMsg); //cambiar nombre y qhable de movement
                    break;
                }
                default: {
                    std::cout << "cmd desconocido: " << cliMsg.event_type << "\n";
                }
            } 
        } catch (const std::out_of_range& e) {
            std::cout << "cmd desconocido: " << "\n";
        } catch (const std::exception& e) {
            std::cerr << "client_main error: " << e.what() << "\n";
        }
        
    }
    listening = false;
}

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

    Op op = cmdToOp.at(cmd);
    CliMsg cliMsg;
    cliMsg.event_type = op;
    cliMsg.movement.steer = 1;
    senderQueue.push(cliMsg);

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