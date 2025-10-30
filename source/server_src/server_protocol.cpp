#include "server_protocol.h"
#include <cstring>
#include <stdexcept>
#include <vector>
#include <arpa/inet.h>

ServerProtocol::ServerProtocol(Socket& peer): peer(peer) {}


int ServerProtocol::sendPlayerState(const SrvMsg& msg) {
    try {
        std::vector<char> buf(sizeof(SrvMsg));
        size_t offset = 0;

        memcpy(buf.data() + offset, &msg.type, sizeof(Opcode));
        offset += sizeof(Opcode);

        memcpy(buf.data() + offset, &msg.posicion.player_id, sizeof(msg.posicion.player_id));
        offset += sizeof(msg.posicion.player_id);
        memcpy(buf.data() + offset, &msg.posicion.x, sizeof(msg.posicion.x));
        offset += sizeof(msg.posicion.x);
        memcpy(buf.data() + offset, &msg.posicion.y, sizeof(msg.posicion.y));
        offset += sizeof(msg.posicion.y);
        memcpy(buf.data() + offset, &msg.posicion.angleRad, sizeof(msg.posicion.angleRad));
        offset += sizeof(msg.posicion.angleRad);


        int n = peer.sendall(buf.data(), offset);
        return n;

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}


Opcode ServerProtocol::recvOpcode() {
    Opcode op{};
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


MoveInfo ServerProtocol::recvMoveInfo() {
    MoveInfo moveInfo;
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