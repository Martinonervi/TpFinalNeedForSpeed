#pragma once
#include <cstring>

#include "../common_src/cli_msg/cli_request_upgrade.h"
#include "../common_src/cli_msg/cli_start_game.h"
#include "../common_src/cli_msg/disconnect_request.h"
#include "../common_src/cli_msg/init_player.h"
#include "../common_src/cli_msg/move_Info.h"
#include "../common_src/cli_msg/requestgame.h"
#include "../common_src/srv_msg/client_disconnect.h"
#include "../common_src/srv_msg/joingame.h"
#include "../common_src/srv_msg/metadatagames.h"
#include "../common_src/srv_msg/new_player.h"
#include "../common_src/srv_msg/player_state.h"
#include "../common_src/srv_msg/playerstats.h"
#include "../common_src/srv_msg/send_player.h"
#include "../common_src/srv_msg/srv_car_hit_msg.h"
#include "../common_src/srv_msg/srv_car_select.h"
#include "../common_src/srv_msg/srv_checkpoint_hit_msg.h"
#include "../common_src/srv_msg/srv_current_info.h"
#include "../common_src/srv_msg/srv_recommended_path.h"
#include "../common_src/srv_msg/srv_send_upgrade.h"
#include "../common_src/srv_msg/srv_time_left.h"
#include "../common_src/srv_msg/srv_upgrade_logic.h"

#include "client_types.h"


class ClientProtocol {
public:

    // Constructor

    explicit ClientProtocol(Socket& peer);


    /* ---------- Serialización de datos ---------- */

    Op readActionByte() const;

    // QT

    void requestGames();
    void sendDisconnectReq(DisconnectReq& dr);
    void requestStats();
    void sendRequestUpgrade(RequestUpgrade& up);

    // SDL

    int sendClientMove(const MoveMsg& moveMsg) const;
    int sendInitPlayer(const InitPlayer& ip) const;
    int sendRequestGame(RequestGame& join_game);
    int sendStartGame(const StartGame& sg);

    /* ---------- Deserialización de datos ---------- */

    PlayerState recvSrvMsg();
    SendPlayer recvSendPlayer();
    NewPlayer recvNewPlayer();
    JoinGame recvGameInfo();
    SrvCurrentInfo recvCurrentInfo();
    UpgradeLogic recvUpgradeLogic();
    RecommendedPath recvRecommendedPath();
    MetadataGames getMetadata();
    GameMetadata readOneGame();
    SrvCarHitMsg recvCollisionEvent();
    SrvCheckpointHitMsg recvCheckpointHitEvent();
    ClientDisconnect recvClientDisconnect();
    PlayerStats recvStats();
    TimeLeft recvTimeLeft();
    SendUpgrade recvUpgrade();
    CarSelect recvCarConfirmation();

private:
    Socket& peer;
};
