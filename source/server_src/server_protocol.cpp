#include "server_protocol.h"

#include <cstring>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include <arpa/inet.h>

ServerProtocol::ServerProtocol(Socket& peer): peer(peer) {}

int ServerProtocol::sendOutMsg(const constants::CliMsg& msg) {
    try {
        std::vector<char> buf(sizeof(constants::Op) + sizeof(constants::CliMsg));
        size_t offset = 0;

        constants::Op op =
                constants::Opcode::ServerMSG;  // un byte, no hace falta revisar endianess
        memcpy(buf.data() + offset, &op, sizeof(constants::Op));
        offset += sizeof(constants::Op);

        constants::Cars_W_Nitro cars_with_nitroBE = htons(msg.cars_with_nitro);
        memcpy(buf.data() + offset, &cars_with_nitroBE, sizeof(constants::Cars_W_Nitro));
        offset += sizeof(constants::Cars_W_Nitro);

        memcpy(buf.data() + offset, &msg.event_type, sizeof(constants::Op));
        offset += sizeof(constants::Op);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

constants::Opcode ServerProtocol::recvMsg() {
    constants::Opcode op{};
    size_t n = 0;
    try {
        n = peer.recvall(&op, sizeof(op));  // lanza en error o cierre parcial
    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return op;
}


constants::MoveInfo ServerProtocol::recvMoveInfo() {
    constants::MoveInfo moveInfo;
    size_t n = 0;

    try {
        n = peer.recvall(&moveInfo.accelerate, sizeof(moveInfo.accelerate));
        n += peer.recvall(&moveInfo.brake, sizeof(moveInfo.brake));
        n += peer.recvall(&moveInfo.steer, sizeof(moveInfo.steer));
        n += peer.recvall(&moveInfo.nitro, sizeof(moveInfo.nitro));

    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return moveInfo;

}