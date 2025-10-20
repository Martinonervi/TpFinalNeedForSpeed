// path: server_src/server_acceptor.cpp
#include "server_acceptor.h"

#include <utility>

#include "../common_src/constants.h"

#include "server_types.h"

void Acceptor::stop() {
    Thread::stop();
    try {
        acceptor.shutdown(2);
    } catch (...) {}
    try {
        acceptor.close();
    } catch (...) {}
}


Acceptor::Acceptor(Socket listen_sock, ClientsRegistry& registry_ref,
                   serv_types::gameLoopQueue& cmd_queue_ref):
        acceptor(std::move(listen_sock)), registry(registry_ref), cmd_queue(cmd_queue_ref) {}


void Acceptor::run() {
    try {
        Main();
    } catch (const std::exception& e) {
        std::cerr << "[Acceptor] fatal: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[Acceptor] fatal: unknown\n";
    }
}


void Acceptor::Main() {
    while (should_keep_running()) {
        try {
            Socket peer = acceptor.accept();

            auto [id, sendq] = registry.AddClient();

            auto h = std::make_unique<ClientHandler>(std::move(peer), id, sendq, cmd_queue);
            h->start();
            handlers.push_back(std::move(h));

            for (auto& it: handlers)
                if (it)
                    it->poll();  // checkeo señal del receiver para cerrar

            reap_dead();
        } catch (...) {
            break;
        }
    }
    kill_all();
}

void Acceptor::reap_dead() {
    for (auto it = handlers.begin(); it != handlers.end();) {
        if (!(*it)->is_alive()) {
            registry.EraseQueue((*it)->getID());
            it = handlers.erase(it);
        } else {
            ++it;
        }
    }
}

void Acceptor::kill_all() {
    for (auto& h: handlers) {
        if (!h)
            continue;
        h->stop();
        h->join();
        registry.EraseQueue(h->getID());
    }
    handlers.clear();
}
