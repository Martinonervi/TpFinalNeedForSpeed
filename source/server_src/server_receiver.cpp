#include "server_receiver.h"

#include <memory>
#include <utility>

Receiver::Receiver(Socket& peer_socket, gameLoopQueue& queue, serv_types::ID clientID):
        peer(peer_socket), cmdQueue(queue), id(clientID), protocol(peer) {}

void Receiver::stop() {
    Thread::stop();
    try {
        peer.shutdown(0);
    } catch (...) {}
}

void Receiver::run() {
    constants::Op op{};
    while (should_keep_running()) {
        try {
            op = protocol.recvMsg();  // bloquea hasta que lee el byte o falla
        } catch (...) {
            peerClosed = true;
            break;
        }
        if (op == constants::Opcode::ClientMSG) {  // 0x04
            cmdQueue.push(serv_types::Cmd{serv_types::CommandType::Nitro, id});
        }
    }
    listening = false;  // hilo terminó
}
