#include "server_receiver.h"
#include "../common_src/constants.h"
#include <memory>
#include <utility>

Receiver::Receiver(Socket& peer_socket, gameLoopQueue& queue, ID clientID):
        peer(peer_socket), cmdQueue(queue), id(clientID), protocol(peer) {}


void Receiver::run() {
    Opcode op{};
    while (should_keep_running()) {
        try {
            op = protocol.recvOpcode();  // bloquea hasta que lee el byte o falla
        } catch (...) {
            peerClosed = true;
            break;
        }

        switch (op) {
            case (Opcode::Movement): {
                MoveInfo moveInfo = protocol.recvMoveInfo();
                cmdQueue.push(Cmd{op, id, moveInfo});
                break;
            }
            default: {
                std::cout << "cmd desconocido: " << op << "\n";
            }
        }

    }
    listening = false;  // hilo terminó
}



void Receiver::stop() {
    Thread::stop();
    try {
        peer.shutdown(0);
    } catch (...) {}
}
