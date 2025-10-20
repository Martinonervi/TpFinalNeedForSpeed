#include "server_client_registry.h"

#include <algorithm>
#include <utility>

ClientsRegistry::ClientsRegistry(): last_id(0) {}

std::pair<ClientsRegistry::ID, ClientsRegistry::SendQPtr> ClientsRegistry::AddClient() {
    auto q = std::make_shared<SendQ>();
    std::lock_guard<std::mutex> lk(mx);
    const ID id = ++last_id;
    clients.emplace(id, q);
    return {id, std::move(q)};
}

void ClientsRegistry::EraseQueue(ID id) {
    std::lock_guard<std::mutex> lk(mx);
    clients.erase(id);
}

int ClientsRegistry::size() const {
    std::lock_guard<std::mutex> lk(mx);
    return clients.size();
}

void ClientsRegistry::broadcast(const constants::OutMsg& msg) {
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
