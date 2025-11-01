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
            op = protocol.recvOpcode();
        } catch (...) {
            peerClosed = true;
            break;
        }

        switch (op) {
            case Opcode::Movement: {
                MoveMsg mv = protocol.recvMoveInfo();  // lo recibe por valor
                // Feli este comentario es para vos:
                // Me parece bien que el protoclo me devuelva el mensaje por valor,
                // y que yo me encargue aca de hacer el casteo, pero tmb podria ser logica
                // del protocolo. Lo dejo a tu criterio.
                CliMsgPtr base = std::static_pointer_cast<CliMsg>(
                        std::make_shared<MoveMsg>(std::move(mv)));
                cmdQueue.push(Cmd{id, base});
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
