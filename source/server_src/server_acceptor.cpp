#include "server_acceptor.h"
#include <utility>
#include "server_types.h"


Acceptor::Acceptor(Socket listen_sock, GameManager& game_manager_ref):
        acceptor(std::move(listen_sock)), game_manager(game_manager_ref) {}


void Acceptor::run() {
    try {
        while (should_keep_running()) {
            try {
                Socket peer = acceptor.accept();

                SendQPtr client_queue = std::make_shared<SendQ>();
                auto h = std::make_unique<ClientHandler>(
                    std::move(peer), ++last_id, client_queue, game_manager);
                h->start();
                handlers.push_back(std::move(h));

                for (auto& it : handlers)
                    if (it) it->poll();

                reap_dead();
            } catch (const std::exception& e) {
                if (!should_keep_running()) {
                    break;
                }
                std::cerr << "[Acceptor] fatal: " << e.what() << "\n";
                break;
            }
        }
    } catch (const std::exception& e) {
        if (should_keep_running()) {
            std::cerr << "[Acceptor] fatal: " << e.what() << "\n";
        }
    }
    kill_all();
}


void Acceptor::reap_dead() {
    for (auto it = handlers.begin(); it != handlers.end();) {
        if (!(*it)->is_alive()) {
            //registry.EraseQueue((*it)->getID());
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
        //registry.EraseQueue(h->getID());
    }
    handlers.clear();
}


void Acceptor::stop() {
    Thread::stop();
    try {
        acceptor.shutdown(2);
    } catch (...) {}
    try {
        acceptor.close();
    } catch (...) {}
}
