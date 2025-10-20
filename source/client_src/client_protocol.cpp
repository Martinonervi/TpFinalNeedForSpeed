#include "client_protocol.h"

#include <arpa/inet.h>

using client_types::RETURN_FAILURE;
using client_types::RETURN_SUCCESS;

ClientProtocol::ClientProtocol(Socket& peer): peer(peer) {}

int ClientProtocol::requestNitro() {
    try {
        constants::Op op = constants::Opcode::ClientMSG;
        peer.sendall(&op, sizeof(constants::Op));

        return RETURN_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        return RETURN_FAILURE;
    }
}

constants::OutMsg ClientProtocol::recvMsg() {
    try {
        if (readActionByte() == constants::Opcode::ServerMSG) {
            // descarto byte
        }
        constants::OutMsg msg;
        constants::Cars_W_Nitro car_w_nBE;

        peer.recvall(&car_w_nBE, sizeof(constants::Cars_W_Nitro));
        msg.cars_with_nitro = ntohs(car_w_nBE);

        peer.recvall(&msg.event_type, sizeof(constants::Op));  // no hace falta revisar endianess

        return msg;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}

constants::Op ClientProtocol::readActionByte() {
    try {
        constants::Op op = constants::Opcode::ClientMSG;
        peer.recvall(&op, sizeof(constants::Op));

        return op;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}
