#include "client_receiver.h"
#include "../common_src/queue.h"

ClientReceiver::ClientReceiver(Socket& peer_sock, Queue<constants::SrvMsg>& receiverQueue)
    :protocol(peer_sock), receiverQueue(receiverQueue){}


void ClientReceiver::run(){
    while (should_keep_running()){
        constants::SrvMsg msg = protocol.recvMsg();
        if (msg.type == constants::Opcode::NitroON) {
            printer.printNitroON();
        } else if (msg.type == constants::Opcode::NitroOFF) {
            printer.printNitroOFF();
        }else if (msg.type == constants::Opcode::Movement) {
            receiverQueue.push(msg);
        } else {
            // nada
        }
    }
}