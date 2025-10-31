#include "client_sender.h"
#include <sstream>

ClientSender::ClientSender(Socket& peer_sock, Queue<CliMsg>& senderQueue)
    :protocol(peer_sock), senderQueue(senderQueue){}

void ClientSender::run(){
    while(should_keep_running()) {
        try{
            CliMsg cliMsg = senderQueue.pop();
            protocol.sendCliMsg(cliMsg); //cambiar nombre y qhable de movement
        } catch (const std::out_of_range& e) {
            std::cout << "cmd desconocido: " << "\n";
        } catch (const std::exception& e) {
            std::cerr << "client_main error: " << e.what() << "\n";
        }
        
    }
    listening = false;
}

bool ClientSender::is_listening() const { return listening; }