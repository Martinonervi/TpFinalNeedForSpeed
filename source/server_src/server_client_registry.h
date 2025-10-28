#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "../common_src/constants.h"
#include "../common_src/queue.h"


// monitor: registra sendQueues por cliente para broadcast seguro
class ClientsRegistry {

public:
    using SendQ = Queue<constants::CliMsg>;
    using SendQPtr = std::shared_ptr<SendQ>;
    using ID = constants::ID;

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
    void broadcast(const constants::CliMsg& msg);

private:
    mutable std::mutex mx;
    ID last_id;
    std::map<ID, SendQPtr> clients;  // client_id → sendQueue del cliente (consumida por el sender)
};
