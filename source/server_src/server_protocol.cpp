#include "server_protocol.h"

#include <cstring>
#include <stdexcept>
#include <vector>

#include <arpa/inet.h>

#include "../common_src/cli_msg/requestgame.h"
#include "../common_src/srv_msg/srv_time_left.h"

ServerProtocol::ServerProtocol(Socket& peer): peer(peer) {}


int ServerProtocol::sendPlayerInit(Player& sp) const {
    try {
        const Op type = sp.type();
        const uint16_t pid = htons(sp.getPlayerId());
        const CarType car = sp.getCarType();
        const uint32_t x_cast = encodeFloatBE(sp.getX());
        const uint32_t y_cast = encodeFloatBE(sp.getY());
        const uint32_t angle  = encodeFloatBE(sp.getAngleRad());

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
        const uint32_t x = encodeFloatBE(ps.getX());
        const uint32_t y = encodeFloatBE(ps.getY());
        const uint32_t angle  = encodeFloatBE(ps.getAngleRad());

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
        uint32_t health_BE = encodeFloatBE(msg.getCarHealth());


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

int ServerProtocol::sendPlayerStats(PlayerStats& msg) {
    try {
        Op type = Opcode::STATS;
        uint8_t racePosition = msg.getRacePosition();
        const uint32_t timeSecToComplete_BE = encodeFloatBE(msg.getTimeSecToComplete());

        std::vector<char> buf(sizeof(Op) + sizeof(racePosition) + sizeof(timeSecToComplete_BE));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &racePosition, sizeof(racePosition));
        offset += sizeof(racePosition);

        memcpy(buf.data() + offset, &timeSecToComplete_BE, sizeof(timeSecToComplete_BE));
        offset += sizeof(timeSecToComplete_BE);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}


int ServerProtocol::sendCurrentInfo(SrvCurrentInfo& msg){
    try {
        const Op type = msg.type();
        const uint32_t speed = encodeFloatBE(msg.getSpeed());
        const uint32_t raceTimeSeconds = encodeFloatBE(msg.getRaceTimeSeconds());
        const uint8_t raceNumber = msg.getRaceNumber();
        const ID nextCheckpointId = htons(msg.getNextCheckpointId());
        const uint32_t checkX = encodeFloatBE(msg.getCheckX());
        const uint32_t checkY = encodeFloatBE(msg.getCheckY());
        const uint32_t angleHint = encodeFloatBE(msg.getAngleHint());
        const uint32_t distanceToChekpoint = encodeFloatBE(msg.getDistanceToCheckpoint());
        const uint8_t totalRaces = msg.getTotalRaces();

        std::vector<char> buf;
        buf.reserve(sizeof(Op) + 2*sizeof(uint8_t) + 7 * sizeof(uint32_t));

        auto append = [&buf](const void* p, std::size_t n) {
            const std::size_t old = buf.size();
            buf.resize(old + n);
            std::memcpy(buf.data() + old, p, n);
        };

        append(&type, sizeof(type));
        append(&speed, sizeof(speed));
        append(&raceTimeSeconds, sizeof(raceTimeSeconds));
        append(&raceNumber, sizeof(raceNumber));
        append(&nextCheckpointId, sizeof(ID));
        append(&checkX, sizeof(checkX));
        append(&checkY, sizeof(checkY));
        append(&angleHint, sizeof(angleHint));
        append(&distanceToChekpoint, sizeof(distanceToChekpoint));
        append(&totalRaces, sizeof(totalRaces));

        return peer.sendall(buf.data(), static_cast<unsigned>(buf.size()));
    } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    throw std::runtime_error("Error sending");
    }
}

int ServerProtocol::sendTimeLeft(TimeLeft& msg) {
    try {
        Op type = Opcode::TIME;
        uint8_t time = msg.getTimeLeft();

        std::vector<char> buf(sizeof(Op) + sizeof(uint8_t));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &time, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

RequestUpgrade ServerProtocol::recvUpgradeReq() {
    Upgrade upgrade;
    int n = 0;
    try {
        n += peer.recvall(&upgrade, sizeof(Op));
    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }

    return RequestUpgrade(upgrade);
}

int ServerProtocol::sendUpgrade(SendUpgrade& up) {
    try{
        Op opcode = up.type();
        Upgrade upgrade = up.getUpgrade();
        bool success = up.couldBuy();

        std::vector<char> buf(sizeof(Op) + sizeof(Op));
        size_t offset = 0;

        memcpy(buf.data() + offset, &opcode, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &upgrade, sizeof(Upgrade));
        offset += sizeof(Upgrade);

        // martino: los bool se mandan bien asi nomas? asumo q se castea a uint*, igual sino
        // nosotros podriamos usar directamente un uint8 como booleano
        memcpy(buf.data() + offset, &success, sizeof(bool));
        offset += sizeof(bool);

        return peer.sendall(buf.data(), offset);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}


int ServerProtocol::sendUpgradeLogic(UpgradeLogic& ul) {
    try{
        Op opcode = ul.type();
        const std::vector<UpgradeDef>& ups = ul.getUpgrades();
        uint8_t size = static_cast<uint8_t>(ups.size());

        const size_t upgradeDefSize =
                sizeof(uint8_t)   // type
                + sizeof(uint32_t)  // value (float codificado)
                + sizeof(uint32_t);  // penalty

        std::vector<char> buf(sizeof(Op) + sizeof(size)
                              + size * upgradeDefSize);
        size_t offset = 0;

        memcpy(buf.data() + offset, &opcode, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &size, sizeof(size));
        offset += sizeof(size);

        for (const auto& upgradeDef : ups) {
            // type -> uint8 (de tipo Op)
            uint8_t t = static_cast<uint8_t>(upgradeDef.type);
            memcpy(buf.data() + offset, &t, sizeof(t));
            offset += sizeof(t);

            const uint32_t value = encodeFloatBE(upgradeDef.value);
            // value (float)
            memcpy(buf.data() + offset, &value, sizeof(value));
            offset += sizeof(value);

            // penalty (float)
            uint32_t penalty = encodeFloatBE(upgradeDef.penaltySec);
            memcpy(buf.data() + offset, &penalty, sizeof(penalty));
            offset += sizeof(penalty);
        }

        return peer.sendall(buf.data(), offset);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

int ServerProtocol::sendRecommendedPath(RecommendedPath& rp) {
    try{
        Op opcode = rp.type();
        const std::vector<RecommendedPoint>& ps = rp.getPath();
        uint8_t size = static_cast<uint8_t>(ps.size());

        const size_t recommendedPointSize =
                sizeof(uint32_t)  // x (float)
                + sizeof(uint32_t);  // y (float)

        std::vector<char> buf(sizeof(Op) + sizeof(size)
                              + size * recommendedPointSize);
        size_t offset = 0;

        memcpy(buf.data() + offset, &opcode, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &size, sizeof(size));
        offset += sizeof(size);

        for (const auto& recommendedPoint : ps) {

            // x (float)
            const uint32_t x = encodeFloatBE(recommendedPoint.x);
            memcpy(buf.data() + offset, &x, sizeof(x));
            offset += sizeof(x);

            // y (float)
            uint32_t y = encodeFloatBE(recommendedPoint.y);
            memcpy(buf.data() + offset, &y, sizeof(y));
            offset += sizeof(y);
        }

        return peer.sendall(buf.data(), offset);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

int ServerProtocol::sendCarConfirmation(CarSelect& car_select) {
    try{
        Op opcode = car_select.type();
        bool success = car_select.isSelected();

        std::vector<char> buf(sizeof(Op) + sizeof(bool));
        size_t offset = 0;

        memcpy(buf.data() + offset, &opcode, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &success, sizeof(Upgrade));
        offset += sizeof(bool);

        return peer.sendall(buf.data(), offset);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}
