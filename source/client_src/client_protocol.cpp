#include "client_protocol.h"

#include <arpa/inet.h>

using client_types::RETURN_FAILURE;
using client_types::RETURN_SUCCESS;

ClientProtocol::ClientProtocol(Socket& peer): peer(peer) {}

int ClientProtocol::requestNitro() const {
    try {
        const constants::Op op = constants::Opcode::Nitro;
        peer.sendall(&op, sizeof(constants::Op));

        return RETURN_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        return RETURN_FAILURE;
    }
}



int ClientProtocol::sendCliMsg(const constants::CliMsg& cliMsg) const {
    try {
        std::vector<char> buf(sizeof(constants::CliMsg));
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


constants::SrvMsg ClientProtocol::recvMsg() {
    try {
        constants::SrvMsg msg;
        peer.recvall(&msg.type, sizeof(constants::Opcode));

        if (msg.type == constants::Opcode::NitroON || msg.type == constants::Opcode::NitroOFF) {
            constants::Cars_W_Nitro car_w_nBE;
            peer.recvall(&car_w_nBE, sizeof(constants::Cars_W_Nitro));
            msg.cars_with_nitro = ntohs(car_w_nBE);
        }

        if (msg.type == constants::Opcode::Movement) {
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

constants::Op ClientProtocol::readActionByte() const {
    try {
        constants::Op op = constants::Opcode::ClientMSG;
        peer.recvall(&op, sizeof(constants::Op));

        return op;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}
