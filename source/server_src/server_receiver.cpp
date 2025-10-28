#include "server_receiver.h"
#include "../common_src/constants.h"
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
    constants::Opcode op{};
    while (should_keep_running()) {
        try {
            op = protocol.recvMsg();  // bloquea hasta que lee el byte o falla
        } catch (...) {
            peerClosed = true;
            break;
        }
        if (op == constants::Opcode::Nitro) {  // 0x04
            cmdQueue.push(constants::Cmd{op, id});
        }
        if (op == constants::Opcode::Movement) {
            constants::MoveInfo moveInfo = protocol.recvMoveInfo();
            std::cout << int(moveInfo.steer) << '\n'; //SE IMPRIME

            cmdQueue.push(constants::Cmd{op, id, moveInfo});

        }
    }
    listening = false;  // hilo terminó
}
