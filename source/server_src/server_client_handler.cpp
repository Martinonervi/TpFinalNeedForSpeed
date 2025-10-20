#include "server_client_handler.h"

#include <utility>


ClientHandler::ClientHandler(Socket peer_sock, const ID id, SendQPtr sendq,
                             gameLoopQueue& cmd_queue):
        id(id),
        peer(std::move(peer_sock)),
        sendq(std::move(sendq)),
        cmd_queue(cmd_queue),
        sender(peer, this->sendq),
        receiver(peer, this->cmd_queue, this->id) {}

void ClientHandler::start() {
    sender.start();
    receiver.start();
}

void ClientHandler::poll() {
    if (receiver.is_peer_closed()) {
        stop();
        join();
    }
}

void ClientHandler::stop() {
    receiver.stop();
    sender.stop();
}

void ClientHandler::join() {
    sender.join();
    receiver.join();
    close();  // ahora nadie usa el socket
}

void ClientHandler::close() {
    try {
        peer.close();
    } catch (...) {}
}

bool ClientHandler::is_alive() { return sender.is_listening() && receiver.is_listening(); }

serv_types::ID ClientHandler::getID() { return id; }
