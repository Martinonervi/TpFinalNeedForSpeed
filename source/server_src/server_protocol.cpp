#include "server_protocol.h"

#include <cstring>
#include <stdexcept>
#include <vector>

#include <arpa/inet.h>

#include "../common_src/cli_msg/requestgame.h"

ServerProtocol::ServerProtocol(Socket& peer): peer(peer) {}


int ServerProtocol::sendPlayerInit(Player& sp) const {
    try {
        const Op type = sp.type();
        const uint16_t pid = htons(sp.getPlayerId());
        const CarType car = sp.getCarType();
        const uint32_t x_cast = encodeFloat100BE(sp.getX());
        const uint32_t y_cast = encodeFloat100BE(sp.getY());
        const uint32_t angle  = encodeFloat100BE(sp.getAngleRad());

        std::vector<char> buf;
        buf.reserve(sizeof(Op) + sizeof(uint16_t) + sizeof(CarType) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));

        auto append = [&buf](const void* p, std::size_t n) {
            const std::size_t old = buf.size();
            buf.resize(old + n);
            std::memcpy(buf.data() + old, p, n);
        };

        append(&type,sizeof(type));
        append(&pid,sizeof(pid));
        append(&car,sizeof(car));
        append(&x_cast,sizeof(uint32_t));
        append(&y_cast, sizeof(uint32_t));
        append(&angle, sizeof(uint32_t));

        return peer.sendall(buf.data(), static_cast<unsigned>(buf.size()));
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw std::runtime_error("Error sending");
    }
}

int ServerProtocol::sendPlayerState(const PlayerState& ps) const {
    try {
        const Op type = Movement;
        const uint16_t pid = htons(ps.getPlayerId());
        const uint32_t x = encodeFloat100BE(ps.getX());
        const uint32_t y = encodeFloat100BE(ps.getY());
        const uint32_t angle  = encodeFloat100BE(ps.getAngleRad());
        const ID nextCheckpointId = htonl(ps.getNextCheckpointId());
        const uint32_t checkX = encodeFloat100BE(ps.getCheckX());
        const uint32_t checkY = encodeFloat100BE(ps.getCheckY());
        const uint32_t hintDirX = encodeFloat100BE(ps.getHintDirX());
        const uint32_t hintDirY = encodeFloat100BE(ps.getHintDirY());

        std::vector<char> buf;
        buf.reserve(sizeof(type) + sizeof(pid) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));

        auto append = [&buf](const void* p, std::size_t n) {
            const std::size_t old = buf.size();
            buf.resize(old + n);
            std::memcpy(buf.data() + old, p, n);
        };

        append(&type,  sizeof(type));
        append(&pid,   sizeof(pid));
        append(&x,     sizeof(x));
        append(&y,     sizeof(y));
        append(&angle, sizeof(angle));

        append(&nextCheckpointId, sizeof(nextCheckpointId));
        append(&checkX, sizeof(checkX));
        append(&checkY, sizeof(checkY));
        append(&hintDirX, sizeof(hintDirX));
        append(&hintDirY, sizeof(hintDirY));

        return peer.sendall(buf.data(), static_cast<unsigned>(buf.size()));
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw std::runtime_error("Error sending");
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
        uint32_t game_id_BE;
        peer.recvall(&game_id_BE, sizeof(uint32_t));
        ID game_id = ntohl(game_id_BE);
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
        uint32_t ID_BE = htonl(game_info.getGameID());

        std::vector<char> buf(sizeof(Op) + sizeof(bool) + sizeof(Op) + sizeof(ID));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &joined, sizeof(joined));
        offset += sizeof(bool);

        memcpy(buf.data() + offset, &code, sizeof(err_code));
        offset += sizeof(err_code);

        memcpy(buf.data() + offset, &ID_BE, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

int ServerProtocol::sendGames(const MetadataGames& games) {
    try {
        Op type = games.type();
        std::vector<char> buf;

        append(buf, &type, sizeof(Op));
        uint32_t size_BE = htonl(static_cast<uint32_t>(games.size()));
        append(buf, &size_BE, sizeof(uint32_t));

        for (const auto& game: games.getMetadata()) {
            writeGameAppend(buf, game);
        }

        int n = peer.sendall(buf.data(), buf.size());
        std::cout << "[Server Protocol] Games sent" << std::endl;
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

void ServerProtocol::append(std::vector<char>& buf, const void* p, std::size_t n) {
    const std::size_t old = buf.size();
    buf.resize(old + n);                  // agranda
    std::memcpy(buf.data() + old, p, n);  // copia al final
}

void ServerProtocol::writeGameAppend(std::vector<char>& buf, const GameMetadata& metadata) {
    const ID game_id_BE  = htonl(metadata.game_id);
    const int players_BE = htonl(metadata.players);
    const bool started = metadata.started;

    append(buf, &game_id_BE, sizeof(ID));
    append(buf, &players_BE, sizeof(int));
    append(buf, &started, sizeof(bool));
}

//endianess
int ServerProtocol::sendCollisionEvent(SrvCarHitMsg& msg){
    try {
        Op type = COLLISION;
        ID player_id_BE = htonl(msg.getPlayerId());
        uint32_t health_BE = encodeFloat100BE(msg.getCarHealth());


        std::vector<char> buf(sizeof(Op) + sizeof(ID) + sizeof(uint32_t));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &player_id_BE, sizeof(ID));
        offset += sizeof(ID);

        memcpy(buf.data() + offset, &health_BE, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

int ServerProtocol::sendCheckpointHit(SrvCheckpointHitMsg& msg) {
    try {
        Op type = CHECKPOINT_HIT;
        ID player_id_BE = htonl(msg.getPlayerId());
        ID checkpoint_id_BE = htonl(msg.getCheckpointId());


        std::vector<char> buf(sizeof(Op) + sizeof(ID) + sizeof(ID));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &player_id_BE, sizeof(ID));
        offset += sizeof(ID);

        memcpy(buf.data() + offset, &checkpoint_id_BE, sizeof(ID));
        offset += sizeof(ID);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}


int ServerProtocol::sendClientDisconnect(ClientDisconnect& msg) {
    try {
        Op type = Opcode::CLIENT_DISCONNECT;
        ID player_id_BE = htonl(msg.getPlayerId());


        std::vector<char> buf(sizeof(Op) + sizeof(ID));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &player_id_BE, sizeof(ID));
        offset += sizeof(ID);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

DisconnectReq ServerProtocol::recvDisconnectReq() {
    size_t n = 0;

    uint32_t ID_BE;

    try {
        n = peer.recvall(&ID_BE, sizeof(uint32_t));
    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }

    ID id = static_cast<ID>(ntohl(ID_BE));
    return DisconnectReq(id);
}