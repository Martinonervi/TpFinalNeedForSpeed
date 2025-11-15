#include "client_protocol.h"

#include <arpa/inet.h>

#include "../common_src/cli_msg/requestgame.h"
#include "../common_src/srv_msg/metadatagames.h"


ClientProtocol::ClientProtocol(Socket& peer): peer(peer) {}

int ClientProtocol::sendInitPlayer(const InitPlayer& ip) const {
    try {
        Op type = INIT_PLAYER;
        //std::string name = ip.getName();
        CarType carType = ip.getCarType();

        std::vector<char> buf(sizeof(CliMsg));
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
        throw("Error sending");
    }

}

int ClientProtocol::sendClientMove(const MoveMsg& moveMsg) const {
    try {
        uint8_t accelerate = moveMsg.getAccelerate();
        uint8_t brake = moveMsg.getBrake();
        int8_t steer = moveMsg.getSteer();
        uint8_t nitro = moveMsg.getNitro();
        Op type = Movement;

        std::vector<char> buf(sizeof(CliMsg));
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
        uint16_t player_id;
        float    x;
        float    y;
        float    angleRad;

        peer.recvall(&player_id, sizeof(player_id));
        peer.recvall(&x, sizeof(x));
        peer.recvall(&y, sizeof(y));
        peer.recvall(&angleRad, sizeof(angleRad));

        //endianess para los floats??
        player_id = ntohs(player_id);

        return PlayerState(player_id, x, y, angleRad);
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

    uint16_t player_id;
    CarType carType;
    float x;
    float y;
    float angleRad;

    try {
        n = peer.recvall(&player_id, sizeof(player_id));
        n += peer.recvall(&carType, sizeof(carType));
        n += peer.recvall(&x, sizeof(x));
        n += peer.recvall(&y, sizeof(y));
        n += peer.recvall(&angleRad, sizeof(angleRad));


    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return SendPlayer(player_id, carType, x, y, angleRad);
}

NewPlayer ClientProtocol::recvNewPlayer() {
    size_t n = 0;

    uint16_t player_id;
    CarType carType;
    float x;
    float y;
    float angleRad;

    try {
        n = peer.recvall(&player_id, sizeof(player_id));
        n += peer.recvall(&carType, sizeof(carType));
        n += peer.recvall(&x, sizeof(x));
        n += peer.recvall(&y, sizeof(y));
        n += peer.recvall(&angleRad, sizeof(angleRad));


    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return NewPlayer(player_id, carType, x, y, angleRad);
}

int ClientProtocol::sendRequestGame(RequestGame& request_game) {
    try {
        Op type = JOIN_GAME;
        ID game_id = request_game.getGameID();

        std::vector<char> buf(sizeof(JoinGame));
        size_t offset = 0;

        memcpy(buf.data() + offset, &type , sizeof(Op));
        offset += sizeof(type);

        memcpy(buf.data() + offset, &game_id , sizeof(ID));
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

        peer.recvall(&joined, sizeof(bool));
        peer.recvall(&exit_code, sizeof(err_code));

        return JoinGame(joined, exit_code);
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
    ID player_id;
    float health;

    try {
        n = peer.recvall(&player_id, sizeof(player_id));
        n += peer.recvall(&health, sizeof(health));

    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return SrvCarHitMsg(player_id, health);
}

SrvCheckpointHitMsg ClientProtocol::recvCheckpointHitEvent(){
    size_t n = 0;
    ID player_id;
    ID checkpoint_id;

    try {
        n = peer.recvall(&player_id, sizeof(player_id));
        n += peer.recvall(&checkpoint_id, sizeof(checkpoint_id));

    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return SrvCheckpointHitMsg(player_id, checkpoint_id);
}

ClientDisconnect ClientProtocol::recvClientDisconnect() {
    size_t n = 0;
    ID player_id;

    try {
        n = peer.recvall(&player_id, sizeof(player_id));

    } catch (...) {
        throw std::runtime_error("recv: closed or error during read");
    }
    if (n == 0) {
        throw std::runtime_error("recv: EOF (0 bytes)");
    }
    return ClientDisconnect(player_id);
}