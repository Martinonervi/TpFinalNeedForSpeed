#include "client_receiver.h"
#include "../common_src/queue.h"

ClientReceiver::ClientReceiver(Socket& peer_sock, Queue<SrvMsg>& receiverQueue)
    :protocol(peer_sock), receiverQueue(receiverQueue){}


void ClientReceiver::run(){
    while (should_keep_running()){
        SrvMsg msg = protocol.recvMsg();
        if (msg.type == Opcode::NitroON) {
            printer.printNitroON();
        } else if (msg.type == Opcode::NitroOFF) {
            printer.printNitroOFF();
        }else if (msg.type == Opcode::Movement) {
            receiverQueue.push(msg);
        } else {
            // nada
        }
    }
}