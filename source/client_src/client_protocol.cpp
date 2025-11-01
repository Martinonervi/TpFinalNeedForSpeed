#include "client_protocol.h"
#include <arpa/inet.h>


ClientProtocol::ClientProtocol(Socket& peer): peer(peer) {}



int ClientProtocol::sendClientMove(const MoveMsg& moveMsg) const {
    try {
        uint8_t accelerate = moveMsg.getAccelerate();
        uint8_t brake = moveMsg.getBrake();
        int8_t steer = moveMsg.getSteer();
        uint8_t nitro = moveMsg.getNitro();
        Op type = moveMsg.type();

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
    try {
        Op op;
        peer.recvall(&op, sizeof(Op));

        return op;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}
