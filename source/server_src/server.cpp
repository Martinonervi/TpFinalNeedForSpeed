#include "server.h"

#include <sstream>
#include <string>
#include <utility>

Server::Server(const char* service):
        game_registry(),
        acc_sock(service),
        acceptor(std::move(acc_sock),game_registry) {}

int Server::run() {
    try {
        //gameloop.start();
        acceptor.start();

        std::string line;
        while (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            char c = 0;
            if (!(ss >> c))
                continue;
            if (c == 'q' || c == 'Q') {
                stop_workers();
                join_workers();
                return RETURN_SUCCESS;
            }
        }
        stop_workers();
        join_workers();

        return RETURN_SUCCESS;
    } catch (...) {
        return RETURN_FAILURE;
    }
}

void Server::stop_workers() {
    //gameloop.stop();
    acceptor.stop();
}
void Server::join_workers() {
    //gameloop.join();
    acceptor.join();
}
