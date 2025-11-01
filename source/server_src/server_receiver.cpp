#include "server_receiver.h"
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
        std::cout << "server receiver: llegue a leer un opcode\n";
        switch (op) {
            case Opcode::Movement: {
                MoveMsg mv = protocol.recvMoveInfo();  // lo recibe por valor
                CliMsgPtr base = std::static_pointer_cast<CliMsg>(
                        std::make_shared<MoveMsg>(std::move(mv)));
                cmdQueue.push(Cmd{id, base});
                break;
            }
            case Opcode::INIT_PLAYER: {
                std::cout << "entre a init player del server receiver\n";
                InitPlayer ip = protocol.recvInitPlayer();
                CliMsgPtr base = std::static_pointer_cast<CliMsg>(
                        std::make_shared<InitPlayer>(std::move(ip)));
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
