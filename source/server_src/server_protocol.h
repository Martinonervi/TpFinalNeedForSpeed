#pragma once
#include <memory>

#include "../common_src/cli_msg/cli_request_upgrade.h"
#include "../common_src/cli_msg/disconnect_request.h"
#include "../common_src/cli_msg/init_player.h"
#include "../common_src/cli_msg/move_Info.h"
#include "../common_src/cli_msg/requestgame.h"
#include "../common_src/srv_msg/client_disconnect.h"
#include "../common_src/srv_msg/joingame.h"
#include "../common_src/srv_msg/metadatagames.h"
#include "../common_src/srv_msg/player.h"
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
#include "../common_src/cli_msg/cli_cheat_request.h"

#include "server_types.h"
#include "../common_src/srv_msg/srv_starting_game.h"


class ServerProtocol {
public:

    // Constructor

    explicit ServerProtocol(Socket& peer);

    /* ---------- Serialización de datos ---------- */

    int sendPlayerState(const PlayerState& ps) const;
    int sendGameInfo(const JoinGame& game_info);
    int sendGames(const MetadataGames& games);
    int sendCollisionEvent(SrvCarHitMsg& msg);
    int sendCheckpointHit(SrvCheckpointHitMsg& msg);
    int sendClientDisconnect(ClientDisconnect& msg);
    int sendCurrentInfo(SrvCurrentInfo& msg);
    int sendUpgradeLogic(UpgradeLogic& ul);
    int sendRecommendedPath(RecommendedPath& rp);
    int sendPlayerInit(Player& sp) const;
    int sendPlayerStats(PlayerStats& msg);
    int sendTimeLeft(TimeLeft& msg);
    int sendUpgrade(SendUpgrade& up);
    int sendCarConfirmation(CarSelect& car_select);
    int sendSartingGame(StartingGame& sg);

    /* ---------- Deserialización de datos ---------- */

    Opcode recvOpcode();
    MoveMsg recvMoveInfo();
    InitPlayer recvInitPlayer();
    RequestGame recvGameInfo();
    DisconnectReq recvDisconnectReq();
    RequestUpgrade recvUpgradeReq();
    CheatRequest recvCheat();

    // Helpers

    void append(std::vector<char>& buf, const void* p, std::size_t n);
    void writeGameAppend(std::vector<char>& buf, const GameMetadata& metadata);

private:

    Socket& peer;

};
