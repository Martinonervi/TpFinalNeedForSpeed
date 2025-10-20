#pragma once

#include <memory>

#include "../common_src/constants.h"
#include "../common_src/socket.h"

#include "server_receiver.h"
#include "server_sender.h"
#include "server_types.h"

class ClientHandler {
public:
    using ID = serv_types::ID;
    using SendQPtr = serv_types::SendQPtr;
    using gameLoopQueue = serv_types::gameLoopQueue;


    ClientHandler(Socket peer_sock, const ID id, SendQPtr sendq, gameLoopQueue& cmd_queue);

    void start();
    void poll();      // Reacciona al flag de EOF del Receiver
    void stop();      // Destraba hilos (NO cierra socket)
    void join();      // Espera hilos y luego cierra el socket
    void close();     // Cierra el socket (centralizado)
    bool is_alive();  // Ambos hilos aún “listening”

    ID getID();

    ClientHandler() = delete;
    ClientHandler(ClientHandler&&) = default;
    ClientHandler& operator=(ClientHandler&&) = default;
    ClientHandler(const ClientHandler&) = delete;
    ClientHandler& operator=(const ClientHandler&) = delete;
    ~ClientHandler() = default;


private:
    ID id{0};
    Socket peer;
    SendQPtr sendq;
    gameLoopQueue& cmd_queue;
    Sender sender;
    Receiver receiver;
};
