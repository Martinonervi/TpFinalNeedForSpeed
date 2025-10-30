#include <iostream>
#include <exception>
#include "common_src/constants.h"
#include "server_src/server.h"

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cerr << "usage: server_main <service>\n";
        return RETURN_FAILURE;
    }
    const char* service = argv[1];

    try {
        Server server(service);
        return server.run();
    } catch (const std::exception& e) {
        std::cerr << "[server] fatal: " << e.what() << "\n";
        return RETURN_FAILURE;
    }
}
