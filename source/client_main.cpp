// path: apps/client_main.cpp
#include "client_src/client.h"
#include <iostream>
#include <exception>
#include <cstdint>

#include "common_src/constants.h"
#include "common_src/socket.h"


int main(int argc, char** argv) {

    if (argc != 3) {
        std::cerr << "usage: client_main <host> <service>\n";
        return RETURN_FAILURE;
    }
    const char* host    = argv[1];
    const char* service = argv[2];

    try {
        Client client(host, service);
        client.run();
        return RETURN_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "[client] error: " << e.what() << "\n";
        return RETURN_FAILURE;
    }
}
