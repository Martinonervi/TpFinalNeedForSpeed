// path: apps/server_main.cpp
#include <iostream>
#include <exception>

#include "common_src/constants.h"
#include "common_src/thread.h"
#include "common_src/queue.h"
#include "common_src/socket.h"

#include "server_src/server.h"

int main(int argc, char** argv) {
    using serv_types::Cmd;

    if (argc != 2) {
        std::cerr << "usage: server_main <service>\n";
        return RETURN_FAILURE;
    }
    const char* service = argv[1];

    try {
        Server s(service);  // constructor pasivo
        return s.Main();
    } catch (const std::exception& e) {
        std::cerr << "[server] fatal: " << e.what() << "\n";
        return RETURN_FAILURE;
    }
}
