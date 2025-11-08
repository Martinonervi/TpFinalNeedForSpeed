#pragma once
#include <cstdint>
#include <list>
#include <memory>
#include "../common_src/constants.h"
#include "server_client_handler.h"
#include "server_types.h"
#include "server_game_manager.h"

class Acceptor: public Thread {

public:
    Acceptor(Socket listen_sock, GameManager& game_manager_ref);
    void stop() override;

protected:
    void run() override;  // loop de aceptación y alta de sesiones

private:
    void reap_dead();  // limpieza oportunista
    void kill_all();   // cierre ordenado de todas las sesiones

    // recursos compartidos (referencias)
    Socket acceptor;            // socket aceptador
    GameManager& game_manager;  // monitor game_id -> game_context

    // lista de clientes
    std::list<std::unique_ptr<ClientHandler>> handlers;
    ID last_id{0};
};
