#include "client_protocol.h"

#include <arpa/inet.h>


ClientProtocol::ClientProtocol(Socket& peer): peer(peer) {}

int ClientProtocol::requestNitro() const {
    try {
        const Op op = Opcode::Nitro;
        peer.sendall(&op, sizeof(Op));

        return RETURN_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        return RETURN_FAILURE;
    }
}



int ClientProtocol::sendCliMsg(const CliMsg& cliMsg) const {
    try {
        std::vector<char> buf(sizeof(CliMsg));
        size_t offset = 0;

        memcpy(buf.data() + offset, &cliMsg.event_type, sizeof(cliMsg.event_type));
        offset += sizeof(cliMsg.event_type);

        //movement.accelerate
        memcpy(buf.data() + offset, &cliMsg.movement.accelerate, sizeof(cliMsg.movement.accelerate));
        offset += sizeof(cliMsg.movement.accelerate);

        //movement.brake
        memcpy(buf.data() + offset, &cliMsg.movement.brake, sizeof(cliMsg.movement.brake));
        offset += sizeof(cliMsg.movement.brake);

        //movement.steer
        memcpy(buf.data() + offset, &cliMsg.movement.steer, sizeof(cliMsg.movement.steer));
        offset += sizeof(cliMsg.movement.steer);

        //movement.nitro
        memcpy(buf.data() + offset, &cliMsg.movement.nitro, sizeof(cliMsg.movement.nitro));
        offset += sizeof(cliMsg.movement.nitro);


        int n = peer.sendall(buf.data(), offset);
        return n;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        throw("Error sending");
    }
}


SrvMsg ClientProtocol::recvMsg() {
    try {
        SrvMsg msg;
        peer.recvall(&msg.type, sizeof(Opcode));

        if (msg.type == Opcode::NitroON || msg.type == Opcode::NitroOFF) {
            Cars_W_Nitro car_w_nBE;
            peer.recvall(&car_w_nBE, sizeof(Cars_W_Nitro));
            msg.cars_with_nitro = ntohs(car_w_nBE);
        }

        if (msg.type == Opcode::Movement) {
            peer.recvall(&msg.posicion.player_id, sizeof(msg.posicion.player_id));
            peer.recvall(&msg.posicion.tick, sizeof(msg.posicion.tick));
            peer.recvall(&msg.posicion.x, sizeof(msg.posicion.x));
            peer.recvall(&msg.posicion.y, sizeof(msg.posicion.y));
            peer.recvall(&msg.posicion.angle_deg, sizeof(msg.posicion.angle_deg));
            peer.recvall(&msg.posicion.vx, sizeof(msg.posicion.vx));
            peer.recvall(&msg.posicion.vy, sizeof(msg.posicion.vy));

        }




        return msg;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}

Op ClientProtocol::readActionByte() const {
    try {
        Op op = Opcode::ClientMSG;
        peer.recvall(&op, sizeof(Op));

        return op;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}
