#include "client_sender.h"

#include <sstream>

ClientSender::ClientSender(Socket& peer_sock, Queue<constants::CliMsg>& senderQueue)
    :protocol(peer_sock), senderQueue(senderQueue){}

//voy a leer de la queue de la interfaz grafica
void ClientSender::run(){
    //std::string line, cmd, param;
    //while (std::getline(std::cin, line) && (parseLine(line, cmd, param) && should_keep_running())) {

    while(should_keep_running()){
        if (!leerStdinYEncolar()) {
            break;
        }
        try{
            constants::CliMsg cliMsg = senderQueue.pop();

            switch (cliMsg.event_type) {
                case (constants::ClientMSG): {
                    protocol.requestNitro();
                    break;
                }
                case (constants::Movement): {
                    protocol.sendCliMsg(cliMsg);
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

    constants::Op op = client_types::cmdToOp.at(cmd);
    constants::CliMsg cliMsg;
    cliMsg.event_type = op;
    senderQueue.push(cliMsg);

    return true;

}

bool ClientSender::parseLine(const std::string& line, std::string& cmd, std::string& param) {
    cmd.clear();  // vaciamos los buffers por las dudas
    param.clear();

    std::istringstream iss(line);
    iss >> cmd;
    std::cout << cmd << '\n';
    if (cmd == EXIT_CMD)
        return false;  // salir del loop si el cliente pone exit
    iss >> param;      // si no encuentro nada no hace nada (param vacio)

    return true;
}

bool ClientSender::is_listening() const { return listening; }