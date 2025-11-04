#include "server_client_registry.h"
#include <algorithm>
#include <utility>

void ClientsRegistry::AddClient(SendQPtr client_queue, ID client_id) {
    std::lock_guard<std::mutex> lk(mx);
    const auto n = clients.size();
    if (n > 8) {
        throw(ERR_GAME_FULL);
    }
    clients.emplace(client_id, client_queue);
}

void ClientsRegistry::EraseQueue(ID id) {
    std::lock_guard<std::mutex> lk(mx);
    clients.erase(id);
}

int ClientsRegistry::size() const { //borrar método, no sirve creo
    std::lock_guard<std::mutex> lk(mx);
    return static_cast<int>(clients.size());
}

void ClientsRegistry::broadcast(const SrvMsgPtr& msg) {
    std::vector<SendQPtr> qs;
    {
        std::lock_guard<std::mutex> lk(mx);
        qs.reserve(clients.size());
        // for (auto& kv: clients) qs.push_back(kv.second);  // snapshot
        std::transform(clients.begin(), clients.end(), std::back_inserter(qs),
                       [](const auto& kv) { return kv.second; });
    }
    for (auto& q: qs) {
        if (!q)
            continue;
        try {
            q->push(msg);
        } catch (const ClosedQueue&) {
            // cliente cerrándose
        }
    }
}

void ClientsRegistry::sendTo(ID client_id, const SrvMsgPtr& msg) {
    SendQPtr q;
    {
        std::lock_guard<std::mutex> lk(mx);
        auto it = clients.find(client_id);
        if (it != clients.end()) {
            q = it->second;
        }
    }
    if (!q) return;

    try {
        q->push(msg);
    } catch (const ClosedQueue&) {
        // cliente cerrándose ,, borrarmos al cliente?
        // EraseQueue(client_id);
    }
}