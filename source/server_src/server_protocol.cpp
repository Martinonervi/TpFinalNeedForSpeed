#include "server_protocol.h"
#include <cstring>
#include <stdexcept>
#include <vector>
#include <arpa/inet.h>

ServerProtocol::ServerProtocol(Socket& peer): peer(peer) {}



int ServerProtocol::sendPlayerState(const PlayerState& ps) const {
    try {

        //endianess para los floats??
        uint16_t player_id = htons(ps.getPlayerId());
        float    x = ps.getX();
        float    y = ps.getY();
        float    angleRad = ps.getAngleRad();
        Op type = ps.type();

        std::vector<char> buf(sizeof(CliMsg));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(type));
        offset += sizeof(type);

        memcpy(buf.data() + offset, &player_id, sizeof(player_id));
        offset += sizeof(player_id);

        memcpy(buf.data() + offset, &x, sizeof(x));
        offset += sizeof(x);

        //movement.steer
        memcpy(buf.data() + offset, &y, sizeof(y));
        offset += sizeof(y);

        //movement.nitro
        memcpy(buf.data() + offset, &angleRad, sizeof(angleRad));
        offset += sizeof(angleRad);

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


MoveMsg ServerProtocol::recvMoveInfo() {
    size_t n = 0;

    uint8_t a;
    uint8_t b;
    int8_t s;
    uint8_t ni;

    try {
        n = peer.recvall(&a, sizeof(a));
        n += peer.recvall(&b, sizeof(b));
        n += peer.recvall(&s, sizeof(s));
        n += peer.recvall(&ni, sizeof(ni));


    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    MoveMsg moveMsg(a,b,s,ni);
    return moveMsg;

}