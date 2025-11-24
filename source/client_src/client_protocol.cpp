#include "client_protocol.h"

#include <arpa/inet.h>

#include "../common_src/cli_msg/disconnect_request.h"
#include "../common_src/cli_msg/requestgame.h"
#include "../common_src/srv_msg/metadatagames.h"
#include "../common_src/srv_msg/playerstats.h"


ClientProtocol::ClientProtocol(Socket& peer): peer(peer) {}

int ClientProtocol::sendInitPlayer(const InitPlayer& ip) const {
    try {
        Op type = INIT_PLAYER;
        //std::string name = ip.getName();
        CarType carType = ip.getCarType();

        std::vector<char> buf(sizeof(type) + sizeof(carType));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type, sizeof(type));
        offset += sizeof(type);

        //memcpy(buf.data() + offset, &name, sizeof(name));
        //offset += sizeof(name);

        memcpy(buf.data() + offset, &carType, sizeof(carType));
        offset += sizeof(carType);

        int n = peer.sendall(buf.data(), offset);
        return n;
    }  catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw std::runtime_error("Error sending");
    }

}

int ClientProtocol::sendClientMove(const MoveMsg& moveMsg) const {
    try {
        uint8_t accelerate = moveMsg.getAccelerate();
        uint8_t brake = moveMsg.getBrake();
        int8_t steer = moveMsg.getSteer();
        uint8_t nitro = moveMsg.getNitro();
        Op type = Movement;

        std::vector<char> buf(sizeof(type) + sizeof(accelerate) + sizeof(brake) + sizeof(steer) + sizeof(nitro));
        size_t offset = 0;

        //type
        memcpy(buf.data() + offset, &type, sizeof(type));
        offset += sizeof(type);

        //accelerate
        memcpy(buf.data() + offset, &accelerate, sizeof(accelerate));
        offset += sizeof(accelerate);

        //brake
        memcpy(buf.data() + offset, &brake, sizeof(brake));
        offset += sizeof(brake);

        //steer
        memcpy(buf.data() + offset, &steer, sizeof(steer));
        offset += sizeof(steer);

        //nitro
        memcpy(buf.data() + offset, &nitro, sizeof(nitro));
        offset += sizeof(nitro);

        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}


PlayerState ClientProtocol::recvSrvMsg() {
    try {
        uint16_t player_id_BE;
        uint32_t x_BE;
        uint32_t y_BE;
        uint32_t angleRad_BE;


        peer.recvall(&player_id_BE, sizeof(uint16_t));
        peer.recvall(&x_BE, sizeof(uint32_t));
        peer.recvall(&y_BE, sizeof(uint32_t));
        peer.recvall(&angleRad_BE, sizeof(uint32_t));


        //endianess para los floats??

        uint16_t player_id = ntohs(player_id_BE);
        float x = decodeFloat100BE(x_BE);
        float y = decodeFloat100BE(y_BE);
        float angleRad = decodeFloat100BE(angleRad_BE);


        PlayerState ps(player_id, x, y, angleRad);
        return ps;

    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}

Op ClientProtocol::readActionByte() const {
    size_t n = 0;
    Op op;
    try {
        n = peer.recvall(&op, sizeof(Op));
    }  catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return op;
}

SendPlayer ClientProtocol::recvSendPlayer() {
    size_t n = 0;

    uint16_t player_id_BE;
    CarType carType;
    uint32_t x_BE;
    uint32_t y_BE;
    uint32_t angleRad_BE;

    try {
        n = peer.recvall(&player_id_BE, sizeof(uint16_t));
        n += peer.recvall(&carType, sizeof(carType));
        n += peer.recvall(&x_BE, sizeof(uint32_t));
        n += peer.recvall(&y_BE, sizeof(uint32_t));
        n += peer.recvall(&angleRad_BE, sizeof(uint32_t));


    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    uint16_t player_id = ntohs(player_id_BE);
    float x = decodeFloat100BE(x_BE);
    float y = decodeFloat100BE(y_BE);
    float angleRad = decodeFloat100BE(angleRad_BE);
    return SendPlayer(player_id, carType, x, y, angleRad);
}

NewPlayer ClientProtocol::recvNewPlayer() {
    size_t n = 0;

    uint16_t player_id_BE;
    CarType carType;
    uint32_t x_BE;
    uint32_t y_BE;
    uint32_t angleRad_BE;

    try {
        n = peer.recvall(&player_id_BE, sizeof(uint16_t));
        n += peer.recvall(&carType, sizeof(carType));
        n += peer.recvall(&x_BE, sizeof(uint32_t));
        n += peer.recvall(&y_BE, sizeof(uint32_t));
        n += peer.recvall(&angleRad_BE, sizeof(uint32_t));
    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    float player_id = ntohs(player_id_BE);
    float x = decodeFloat100BE(x_BE);
    float y = decodeFloat100BE(y_BE);
    float angleRad = decodeFloat100BE(angleRad_BE);

    return NewPlayer(player_id, carType, x, y, angleRad);
}

int ClientProtocol::sendRequestGame(RequestGame& request_game) {
    try {
        Op type = JOIN_GAME;
        ID game_id_BE = htonl(request_game.getGameID());

        std::vector<char> buf(sizeof(Op) + sizeof(ID));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type , sizeof(Op));
        offset += sizeof(type);

        memcpy(buf.data() + offset, &game_id_BE , sizeof(ID));
        offset += sizeof(ID);

        int n = peer.sendall(buf.data(), offset);
        return n;
    }  catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

JoinGame ClientProtocol::recvGameInfo() {
    try {
        bool joined;
        err_code exit_code;
        uint32_t game_id_BE;

        peer.recvall(&joined, sizeof(bool));
        peer.recvall(&exit_code, sizeof(err_code));
        peer.recvall(&game_id_BE, sizeof(uint32_t));

        ID game_id = static_cast<ID>(ntohl(game_id_BE));

        return JoinGame(joined, exit_code, game_id);
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}

void ClientProtocol::requestGames() {
    try {
        Op type = REQUEST_GAMES;
        std::vector<char> buf(sizeof(Op));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type , sizeof(Op));
        offset += sizeof(type);

        peer.sendall(buf.data(), offset);
    }  catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

MetadataGames ClientProtocol::getMetadata() {
    try {
        uint32_t len_BE = 0;
        peer.recvall(&len_BE, sizeof(uint32_t));
        uint32_t len = ntohl(len_BE);
        
        std::vector<GameMetadata> games;
        games.reserve(len);

        for (int i = 0; i < len; i++) {
            games.push_back(readOneGame());
        }

        return MetadataGames(games);
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}

GameMetadata ClientProtocol::readOneGame() {
    GameMetadata out;
    ID game_id_BE;
    uint32_t players_BE;
    bool started;

    peer.recvall(&game_id_BE, sizeof(ID));
    peer.recvall(&players_BE, sizeof(uint32_t));
    peer.recvall(&started, sizeof(bool));


    out.game_id = ntohl(game_id_BE);
    out.players = ntohl(players_BE);
    out.started = started;
    return out;
}

SrvCarHitMsg ClientProtocol::recvCollisionEvent(){
    size_t n = 0;
    ID player_id_BE;
    uint32_t health_BE;

    try {
        n = peer.recvall(&player_id_BE, sizeof(uint32_t));
        n += peer.recvall(&health_BE, sizeof(uint32_t));

    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }

    ID player_id = ntohl(player_id_BE);
    float health = decodeFloat100BE(health_BE);
    return SrvCarHitMsg(player_id, health);
}

SrvCheckpointHitMsg ClientProtocol::recvCheckpointHitEvent(){
    size_t n = 0;
    ID player_id_BE;
    ID checkpoint_id_BE;

    try {
        n = peer.recvall(&player_id_BE, sizeof(ID));
        n += peer.recvall(&checkpoint_id_BE, sizeof(ID));

    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }

    ID player_id = ntohl(player_id_BE);
    ID checkpoint_id = ntohl(checkpoint_id_BE);
    return SrvCheckpointHitMsg(player_id, checkpoint_id);
}

ClientDisconnect ClientProtocol::recvClientDisconnect() {
    size_t n = 0;
    ID player_id_BE;

    try {
        n = peer.recvall(&player_id_BE, sizeof(ID));

    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    ID player_id = ntohl(player_id_BE);
    return ClientDisconnect(player_id);
}

void ClientProtocol::sendDisconnectReq(DisconnectReq& dr) {
    try{
        Op opcode = dr.type();
        ID game_id = htonl(dr.getGameID());

        std::vector<char> buf(sizeof(Op) + sizeof(uint32_t));
        size_t offset = 0;

        memcpy(buf.data() + offset, &opcode, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &game_id, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        peer.sendall(buf.data(), offset);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}


SrvCurrentInfo ClientProtocol::recvCurrentInfo() {
    try {
        uint32_t speed_BE;
        uint32_t raceTimeSeconds_BE;
        uint8_t raceNumber;
        ID nextCheckpointId_BE;
        uint32_t checkX_BE;
        uint32_t checkY_BE;
        uint32_t angleHint_BE;
        uint32_t distanceToChekpoint_BE;
        uint8_t totalRaces;

        peer.recvall(&speed_BE, sizeof(speed_BE));
        peer.recvall(&raceTimeSeconds_BE, sizeof(raceTimeSeconds_BE));
        peer.recvall(&raceNumber, sizeof(raceNumber));
        peer.recvall(&nextCheckpointId_BE, sizeof(nextCheckpointId_BE));
        peer.recvall(&checkX_BE, sizeof(checkX_BE));
        peer.recvall(&checkY_BE, sizeof(checkY_BE));
        peer.recvall(&angleHint_BE, sizeof(angleHint_BE));
        peer.recvall(&distanceToChekpoint_BE, sizeof(distanceToChekpoint_BE));
        peer.recvall(&totalRaces, sizeof(totalRaces));

        float speed = decodeFloat100BE(speed_BE);
        float raceTimeSecond = decodeFloat100BE(raceTimeSeconds_BE);
        ID nextCheckpointID = ntohs(nextCheckpointId_BE);
        float checkX = decodeFloat100BE(checkX_BE);
        float checkY = decodeFloat100BE(checkY_BE);
        float angleHint = decodeFloat100BE(angleHint_BE);
        float distanceToheckpoint = decodeFloat100BE(distanceToChekpoint_BE);


        return SrvCurrentInfo(nextCheckpointID, checkX, checkY, angleHint,
                          distanceToheckpoint, raceTimeSecond, raceNumber, speed, totalRaces);

    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}

void ClientProtocol::requestStats() {
    try {
        Op type = STATS;
        std::vector<char> buf(sizeof(Op));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type , sizeof(Op));
        offset += sizeof(type);

        peer.sendall(buf.data(), offset);
    }  catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

PlayerStats ClientProtocol::recvStats() {
    uint8_t racePosition;
    uint32_t timeSecToComplete_BE;

    int n = 0;
    try {
        n += peer.recvall(&racePosition, sizeof(racePosition));
        n += peer.recvall(&timeSecToComplete_BE, sizeof(timeSecToComplete_BE));

    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }

    float timeSecToComplete = decodeFloat100BE(timeSecToComplete_BE);
    return PlayerStats(racePosition, timeSecToComplete);
}

TimeLeft ClientProtocol::recvTimeLeft() {
    uint8_t time;
    int n = 0;
    try {
        n += peer.recvall(&time, sizeof(uint8_t));
    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return TimeLeft(time);
}

void ClientProtocol::sendRequestUpgrade(RequestUpgrade& up) {
    try{
        Op opcode = up.type();
        Upgrade upgrade = up.getUpgrade();

        std::vector<char> buf(sizeof(Op) + sizeof(Op));
        size_t offset = 0;

        memcpy(buf.data() + offset, &opcode, sizeof(Op));
        offset += sizeof(Op);

        memcpy(buf.data() + offset, &upgrade, sizeof(Upgrade));
        offset += sizeof(Upgrade);

        peer.sendall(buf.data(), offset);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

SendUpgrade ClientProtocol::recvUpgrade() {
    Upgrade upgrade;
    bool success;
    int n = 0;
    try {
        n += peer.recvall(&upgrade, sizeof(Op));
        n += peer.recvall(&success, sizeof(bool));
    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }

    return SendUpgrade(upgrade, success);
}



UpgradeLogic ClientProtocol::recvUpgradeLogic() {
    uint8_t size;
    std::vector<UpgradeDef> uds;

    int n = 0;
    try {
        n += peer.recvall(&size, sizeof(size));
        uds.reserve(size);
        for (uint8_t i = 0; i < size; i++) {
            UpgradeDef ud;

            uint8_t t;
            n += peer.recvall(&t, sizeof(t));
            ud.type = static_cast<Upgrade>(t);

            uint32_t encodedValue;
            n += peer.recvall(&encodedValue, sizeof(encodedValue));
            ud.value = decodeFloat100BE(encodedValue);

            uint32_t encodedPenalty;
            n += peer.recvall(&encodedPenalty, sizeof(encodedPenalty));
            ud.penaltySec = decodeFloat100BE(encodedPenalty);

            uds.push_back(std::move(ud));
        }
    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return UpgradeLogic(std::move(uds));
}

RecommendedPath ClientProtocol::recvRecommendedPath() {
    uint8_t size;
    std::vector<RecommendedPoint> rps;

    int n = 0;
    try {
        n += peer.recvall(&size, sizeof(size));
        rps.reserve(size);
        for (uint8_t i = 0; i < size; i++) {
            RecommendedPoint rp;

            uint32_t encodedX;
            n += peer.recvall(&encodedX, sizeof(encodedX));
            rp.x = decodeFloat100BE(encodedX);

            uint32_t encodedY;
            n += peer.recvall(&encodedY, sizeof(encodedY));
            rp.y = decodeFloat100BE(encodedY);

            rps.push_back(std::move(rp));
        }
    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return RecommendedPath(std::move(rps));
}


int ClientProtocol::sendStartGame(const StartGame& sg) {
    try {
        Op type = sg.type();

        std::vector<char> buf(sizeof(Op));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type , sizeof(Op));
        offset += sizeof(type);

        int n = peer.sendall(buf.data(), offset);
        return n;
    }  catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}

CarSelect ClientProtocol::recvCarConfirmation() {
    bool confirmation;

    int n = 0;
    try {
        n += peer.recvall(&confirmation, sizeof(bool));
    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }

    return CarSelect(confirmation);
}
