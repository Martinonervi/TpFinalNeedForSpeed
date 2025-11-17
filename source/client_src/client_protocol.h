#pragma once
#include <cstring>

#include "../common_src/cli_msg/init_player.h"
#include "../common_src/cli_msg/move_Info.h"
#include "../common_src/cli_msg/requestgame.h"
#include "../common_src/cli_msg/disconnect_request.h"
#include "../common_src/srv_msg/client_disconnect.h"
#include "../common_src/srv_msg/joingame.h"
#include "../common_src/srv_msg/metadatagames.h"
#include "../common_src/srv_msg/new_player.h"
#include "../common_src/srv_msg/player_state.h"
#include "../common_src/srv_msg/send_player.h"
#include "../common_src/srv_msg/srv_car_hit_msg.h"
#include "../common_src/srv_msg/srv_checkpoint_hit_msg.h"

#include "client_types.h"


class ClientProtocol {
public:
    explicit ClientProtocol(Socket& peer);


    // recibe y devuelve el struct de msg
    PlayerState recvSrvMsg();

    // lee (y por ahora descarta) el primer byte
    Op readActionByte() const;
    int sendClientMove(const MoveMsg& moveMsg) const;
    int sendInitPlayer(const InitPlayer& ip) const;
    int sendRequestGame(RequestGame& join_game);
    SendPlayer recvSendPlayer();
    NewPlayer recvNewPlayer();
    JoinGame recvGameInfo();
    void requestGames();
    MetadataGames getMetadata();
    GameMetadata readOneGame();
    SrvCarHitMsg recvCollisionEvent();
    SrvCheckpointHitMsg recvCheckpointHitEvent();
    ClientDisconnect recvClientDisconnect();
    void sendDisconnectReq(DisconnectReq& dr);


private:
    Socket& peer;
};
