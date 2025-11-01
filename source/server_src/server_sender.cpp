#include "server_sender.h"

#include <memory>
#include <utility>
#include <vector>
#include "../common_src/send_player.h"

Sender::Sender(Socket& peer_socket, SendQPtr queue):
        peer(peer_socket), msg_queue(std::move(queue)), protocol(peer) {}

void Sender::stop() {
    Thread::stop();
    if (msg_queue)
        msg_queue->close();
    try {
        peer.shutdown(1);
    } catch (...) {}
}
void Sender::run() {
    try {
        while (should_keep_running()) {
            SrvMsgPtr msg = msg_queue->pop();

            int n;

            switch (msg->type()) {
                case Opcode::Movement: {
                    n = protocol.sendPlayerState(dynamic_cast<const PlayerState&>(*msg));
                    break;
                }
                case Opcode::INIT_PLAYER: {
                    n = protocol.sendPlayerInit(dynamic_cast<const SendPlayer&>(*msg));
                    break;
                }
                default: {
                    std::cout << "cmd desconocido: " << msg->type() << "\n";
                    n = 0; //quiero salir
                }
            }

            if (n == 0)
                break;
        }
    } catch (const ClosedQueue&) {
        // se cerró la cola mientras esperaba --> client handler lo interrumpió
    } catch (...) {
        // error sending probably
    }
    listening = false;
}


