#pragma once
#include <cstring>

#include "../common_src/init_player.h"
#include "../common_src/joingame.h"
#include "../common_src/move_Info.h"
#include "../common_src/new_player.h"
#include "../common_src/player_state.h"
#include "../common_src/requestgame.h"
#include "../common_src/send_player.h"
#include "../common_src/metadatagames.h"
#include "../common_src/srv_car_hit_msg.h"

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

private:
    Socket& peer;
};
