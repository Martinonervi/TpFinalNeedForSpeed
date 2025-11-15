#include "server_protocol.h"

#include <cstring>
#include <stdexcept>
#include <vector>

#include <arpa/inet.h>

#include "../common_src/requestgame.h"

ServerProtocol::ServerProtocol(Socket& peer): peer(peer) {}


int ServerProtocol::sendPlayerInit(Player& sp) const {
    try {
        // OJO: definir un wire-format claro. Acá empaquetamos:
        // Op (1? 4? según tu enum) + uint16_t id + CarType + float x + float y + float angle
        const Op type      = sp.type();
        const uint16_t pid = /*htons*/(sp.getPlayerId());  // decidir si querés network order
        const CarType car  = sp.getCarType();
        const float x      = sp.getX();
        const float y      = sp.getY();
        const float angle  = sp.getAngleRad();

        std::vector<char> buf;
        buf.reserve(sizeof(type) + sizeof(pid) + sizeof(car) + sizeof(x) + sizeof(y) + sizeof(angle));

        auto append = [&buf](const void* p, std::size_t n) {
            const std::size_t old = buf.size();
            buf.resize(old + n);
            std::memcpy(buf.data() + old, p, n);
        };

        append(&type,  sizeof(type));
        append(&pid,   sizeof(pid));
        append(&car,   sizeof(car));
        append(&x,     sizeof(x));
        append(&y,     sizeof(y));
        append(&angle, sizeof(angle));

        return peer.sendall(buf.data(), static_cast<unsigned>(buf.size()));
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw std::runtime_error("Error sending");
    }
}

int ServerProtocol::sendPlayerState(const PlayerState& ps) const {
    try {
        const Op type      = Movement;
        const uint16_t pid = htons(ps.getPlayerId());
        const float x      = ps.getX();
        const float y      = ps.getY();
        const float angle  = ps.getAngleRad();

        std::vector<char> buf;
        buf.reserve(sizeof(type) + sizeof(pid) + sizeof(x) + sizeof(y) + sizeof(angle));

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
        ID player_id = msg.getPlayerId();
        float health = msg.getCarHealth();


        std::vector<char> buf(sizeof(Op) + sizeof(player_id) + sizeof(health));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &player_id, sizeof(player_id));
        offset += sizeof(player_id);

        memcpy(buf.data() + offset, &health, sizeof(health));
        offset += sizeof(health);

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
        ID player_id = msg.getPlayerId();
        ID checkpoint_id = msg.getCheckpointId();


        std::vector<char> buf(sizeof(Op) + sizeof(player_id) + sizeof(checkpoint_id));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &player_id, sizeof(player_id));
        offset += sizeof(player_id);

        memcpy(buf.data() + offset, &checkpoint_id, sizeof(checkpoint_id));
        offset += sizeof(checkpoint_id);

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
        ID player_id = msg.getPlayerId();


        std::vector<char> buf(sizeof(Op) + sizeof(player_id));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &player_id, sizeof(player_id));
        offset += sizeof(player_id);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}