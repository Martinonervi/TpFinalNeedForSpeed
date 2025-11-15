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

    ClientsRegistry() = default;
    ClientsRegistry(const ClientsRegistry&) = delete;
    ClientsRegistry& operator=(const ClientsRegistry&) = delete;

    // alta: registra la cola de salida para broadcast y mensajes

    // ID AddQueue(SendQPtr q);
    void AddClient(SendQPtr client_queue, ID client_id);
    // baja: remueve si existe
    void EraseQueue(ID id);
    // tamaño actual
    int size() const;
    bool contains(ID id) const;

    // broadcast
    void broadcast(const SrvMsgPtr& msg);
    void sendTo(ID client_id, const SrvMsgPtr& msg);

    std::vector<ID> checkClients(std::vector<ID>& ids);

private:
    mutable std::mutex mx;
    std::map<ID, SendQPtr> clients;  // client_id → sendQueue del cliente (consumida por el sender)
};
