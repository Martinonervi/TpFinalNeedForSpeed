#include "client_protocol.h"

#include <arpa/inet.h>

using client_types::RETURN_FAILURE;
using client_types::RETURN_SUCCESS;

ClientProtocol::ClientProtocol(Socket& peer): peer(peer) {}

int ClientProtocol::requestNitro() {
    try {
        constants::Op op = constants::Opcode::Nitro;
        peer.sendall(&op, sizeof(constants::Op));

        return RETURN_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        return RETURN_FAILURE;
    }
}


int ClientProtocol::sendCliMsg(const constants::CliMsg& cliMsg) {
    try {
        std::vector<char> buf(sizeof(constants::CliMsg));
        size_t offset = 0;

        //eventType
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


constants::CliMsg ClientProtocol::recvMsg() {
    try {
        if (readActionByte() == constants::Opcode::ServerMSG) {
            // descarto byte
        }
        constants::CliMsg msg;
        constants::Cars_W_Nitro car_w_nBE;

        peer.recvall(&car_w_nBE, sizeof(constants::Cars_W_Nitro));
        msg.cars_with_nitro = ntohs(car_w_nBE);

        peer.recvall(&msg.event_type, sizeof(constants::Op));  // no hace falta revisar endianess

        return msg;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}

constants::Op ClientProtocol::readActionByte() {
    try {
        constants::Op op = constants::Opcode::ClientMSG;
        peer.recvall(&op, sizeof(constants::Op));

        return op;
    } catch (const std::exception& e) {
        std::cerr << "client_main error: " << e.what() << "\n";
        throw RETURN_FAILURE;
    }
}
