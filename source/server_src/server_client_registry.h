#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include "../common_src/constants.h"
#include "server_types.h"
#include "../common_src/queue.h"


class ClientsRegistry {

public:

    ClientsRegistry();
    ClientsRegistry(const ClientsRegistry&) = delete;
    ClientsRegistry& operator=(const ClientsRegistry&) = delete;

    // alta: registra la cola de salida y devuelve un id único

    // ID AddQueue(SendQPtr q);
    std::pair<ID, SendQPtr> AddClient();
    // baja: remueve si existe
    void EraseQueue(ID id);
    // tamaño actual
    int size() const;

    // broadcast
    void broadcast(const SrvMsgPtr& msg);
    void sendTo(ID client_id, const SrvMsgPtr& msg);

private:
    mutable std::mutex mx;
    ID last_id;
    std::map<ID, SendQPtr> clients;  // client_id → sendQueue del cliente (consumida por el sender)
};
