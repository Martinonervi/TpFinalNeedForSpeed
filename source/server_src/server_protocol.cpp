#include "server_protocol.h"

#include <cstring>
#include <stdexcept>
#include <vector>

#include <arpa/inet.h>

#include "../common_src/requestgame.h"

ServerProtocol::ServerProtocol(Socket& peer): peer(peer) {}


int ServerProtocol::sendPlayerInit(Player& sp) const {
    try {

        //endianess para los floats?? feli fijate
        uint16_t player_id = sp.getPlayerId()/*htons(sp.getPlayerId())*/;
        CarType carType = sp.getCarType();
        float x = sp.getX();
        float y = sp.getY();
        float angleRad = sp.getAngleRad();
        Op type = sp.type();

        std::vector<char> buf(sizeof(CliMsg));
        size_t offset = 0;

        // type
        memcpy(buf.data() + offset, &type, sizeof(type));
        offset += sizeof(type);

        // player_id
        memcpy(buf.data() + offset, &player_id, sizeof(player_id));
        offset += sizeof(player_id);

        // carType
        memcpy(buf.data() + offset, &carType, sizeof(carType));
        offset += sizeof(carType);

        // x
        memcpy(buf.data() + offset, &x, sizeof(x));
        offset += sizeof(x);

        // y
        memcpy(buf.data() + offset, &y, sizeof(y));
        offset += sizeof(y);

        // angleRad
        memcpy(buf.data() + offset, &angleRad, sizeof(angleRad));
        offset += sizeof(angleRad);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

int ServerProtocol::sendPlayerState(const PlayerState& ps) const {
    try {

        //endianess para los floats?? feli fijate
        uint16_t player_id = htons(ps.getPlayerId());
        float x = ps.getX();
        float y = ps.getY();
        float angleRad = ps.getAngleRad();
        Op type = Movement;

        std::vector<char> buf(sizeof(CliMsg));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(type));
        offset += sizeof(type);

        memcpy(buf.data() + offset, &player_id, sizeof(player_id));
        offset += sizeof(player_id);

        memcpy(buf.data() + offset, &x, sizeof(x));
        offset += sizeof(x);

        memcpy(buf.data() + offset, &y, sizeof(y));
        offset += sizeof(y);

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
    Opcode op;
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

InitPlayer ServerProtocol::recvInitPlayer() {
    size_t n = 0;

    //std::string name;
    CarType carType;

    try {
        //n = peer.recvall(&name, sizeof(name));
        n += peer.recvall(&carType, sizeof(carType));

    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    InitPlayer ip("name", carType);
    return ip;
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

RequestGame ServerProtocol::recvGameInfo() {
    try {
        ID game_id;
        peer.recvall(&game_id, sizeof(ID));
        return RequestGame(game_id);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

int ServerProtocol::sendGameInfo(const JoinGame& game_info) {
    try {
        Op type = JOIN_GAME;
        bool joined = game_info.couldJoin();
        err_code code = game_info.getExitStatus();

        std::vector<char> buf(sizeof(Op) + sizeof(bool) + sizeof(Op));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &joined, sizeof(joined));
        offset += sizeof(bool);

        memcpy(buf.data() + offset, &code, sizeof(err_code));
        offset += sizeof(err_code);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

