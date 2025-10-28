#include "client_receiver.h"
#include "../common_src/queue.h"

ClientReceiver::ClientReceiver(Socket& peer_sock, Queue<std::string>& recv_queue)
    :protocol(peer_sock), recv_queue(recv_queue){}


void ClientReceiver::run(){
    while (should_keep_running()){
        constants::SrvMsg msg = protocol.recvMsg();
        if (msg.type == constants::Opcode::NitroON) {
            printer.printNitroON();
            recv_queue.push("Alguien le dio al nitro");
        } else if (msg.type == constants::Opcode::NitroOFF) {
            printer.printNitroOFF();
            recv_queue.push("Alguien mato el nitro");
        }else if (msg.type == constants::Opcode::Movement) {
            std::cout << "llegue al client receiver por movement, pase por player id: " <<
                    msg.posicion.player_id << "\n";
        } else {
            // nada
        }
    }
}