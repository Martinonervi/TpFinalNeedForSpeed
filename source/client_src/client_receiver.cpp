#include "client_receiver.h"

ClientReceiver::ClientReceiver(Socket& peer_sock)
    :protocol(peer_sock){}


void ClientReceiver::run(){
    while (should_keep_running()){
        constants::OutMsg msg = protocol.recvMsg();
        if (msg.event_type == constants::Opcode::NitroON) {
            printer.printNitroON();
        } else if (msg.event_type == constants::Opcode::NitroOFF) {
            printer.printNitroOFF();
        } else {
            // nada
        }
    }
}