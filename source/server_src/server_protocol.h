#pragma once
#include <memory>

#include "../common_src/init_player.h"
#include "../common_src/joingame.h"
#include "../common_src/move_Info.h"
#include "../common_src/player.h"
#include "../common_src/player_state.h"
#include "../common_src/send_player.h"

#include "server_types.h"

class ServerProtocol {
public:
    explicit ServerProtocol(Socket& peer);

    // serializa y envía el mensaje por el socket
    int sendPlayerState(const PlayerState& ps) const;

    // recibe mensaje y devuelve el opcode del mensaje recibido
    Opcode recvOpcode();

    MoveMsg recvMoveInfo();

    InitPlayer recvInitPlayer();

    int sendPlayerInit(Player& sp) const;

    JoinGame getGameInfo();

private:
    Socket& peer;

};
