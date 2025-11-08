#include "server_client_handler.h"

#include <utility>

#include <sys/socket.h>


ClientHandler::ClientHandler(Socket peer_sock, const ID id, SendQPtr sendq, GameManager& game_manager_ref):
        id(id),
        game_manager(game_manager_ref),
        peer(std::move(peer_sock)),
        sender(peer, sendq),
        receiver(peer, this->id, game_manager, sendq) {}

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

ID ClientHandler::getID() { return id; }
