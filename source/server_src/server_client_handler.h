#pragma once

#include <memory>

#include "../common_src/constants.h"
#include "../common_src/socket.h"

#include "server_game_manager.h"
#include "server_receiver.h"
#include "server_sender.h"
#include "server_types.h"

class ClientHandler {
public:
    ClientHandler(Socket peer_sock, ID id, SendQPtr sendq, GameManager& game_manager_ref);

    void start();
    void poll();      // Reacciona al flag de EOF del Receiver
    void stop();      // Destraba hilos (NO cierra socket)
    void join();      // Espera hilos y luego cierra el socket
    void close();     // Cierra el socket (centralizado)
    bool is_alive();  // Ambos hilos aún “listening”

    ID getID();

    ClientHandler() = delete;
    ClientHandler(ClientHandler&&) = delete;
    ClientHandler& operator=(ClientHandler&&) = delete;
    ClientHandler(const ClientHandler&) = delete;
    ClientHandler& operator=(const ClientHandler&) = delete;
    ~ClientHandler() = default;


private:
    ID id{0};
    GameManager& game_manager;
    Socket peer;
    Sender sender;
    Receiver receiver;
};
