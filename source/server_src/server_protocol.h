#pragma once
#include <memory>

#include "../common_src/cli_msg/init_player.h"
#include "../common_src/cli_msg/move_Info.h"
#include "../common_src/cli_msg/requestgame.h"
#include "../common_src/srv_msg/client_disconnect.h"
#include "../common_src/srv_msg/joingame.h"
#include "../common_src/srv_msg/metadatagames.h"
#include "../common_src/srv_msg/player.h"
#include "../common_src/srv_msg/player_state.h"
#include "../common_src/srv_msg/send_player.h"
#include "../common_src/srv_msg/srv_car_hit_msg.h"
#include "../common_src/srv_msg/srv_checkpoint_hit_msg.h"

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

    RequestGame recvGameInfo();

    int sendGameInfo(const JoinGame& game_info);

    int sendGames(const MetadataGames& games);

    int sendCollisionEvent(SrvCarHitMsg& msg);

    int sendCheckpointHit(SrvCheckpointHitMsg& msg);

    int sendClientDisconnect(ClientDisconnect& msg);

    void append(std::vector<char>& buf, const void* p, std::size_t n);

    void writeGameAppend(std::vector<char>& buf, const GameMetadata& metadata);

private:
    Socket& peer;

};
