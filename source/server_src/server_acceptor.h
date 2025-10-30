#pragma once
#include <cstdint>
#include <list>
#include <memory>
#include "../common_src/constants.h"
#include "server_client_handler.h"
#include "server_client_registry.h"
#include "server_types.h"

class Acceptor: public Thread {

public:
    Acceptor(Socket listen_sock, ClientsRegistry& registry, gameLoopQueue& cmd_queue);
    void stop() override;

protected:
    void run() override;  // loop de aceptación y alta de sesiones

private:
    void reap_dead();  // limpieza oportunista
    void kill_all();   // cierre ordenado de todas las sesiones

    // recursos compartidos (referencias)
    Socket acceptor;            // socket aceptador
    ClientsRegistry& registry;  // monitor id -> sendQueue
    gameLoopQueue& cmd_queue;   // cola global de comandos

    // lista de clientes
    std::list<std::unique_ptr<ClientHandler>> handlers;
};
