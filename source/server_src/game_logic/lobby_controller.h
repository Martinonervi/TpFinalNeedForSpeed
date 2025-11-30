#ifndef TPFINALNEEDFORSPEED_LOBBY_CONTROLLER_H
#define TPFINALNEEDFORSPEED_LOBBY_CONTROLLER_H

#include <memory>
#include <list>
#include <cstdint>
#include <functional>
#include "../server_types.h"
#include "../server_client_registry.h"
#include "../world/entities/car.h"
#include "../world/map_parser.h"        // RecommendedPoint, SpawnPointConfig
#include "player_manager.h"
#include "config/config_parser.h"
#include "../../common_src/constant_rate_loop.h"
#include "../../common_src/srv_msg/srv_starting_game.h"
#include "../../common_src/srv_msg/srv_time_left.h"
#include "../../common_src/srv_msg/srv_upgrade_logic.h"
#include "../../common_src/srv_msg/srv_recommended_path.h"


class LobbyController {
public:
    LobbyController(std::shared_ptr<gameLoopQueue> queue,
                    ClientsRegistry& registry,
                    PlayerManager& playerManager,
                    std::unordered_map<ID, Car>& playerCars,
                    const std::vector<UpgradeDef>& upgrades,
                    const Config& config,
                    bool& startRequested,
                    bool& raceStarted,
                    uint8_t& totalCars);

    void runLobbyLoop(int raceIndex,
                      const std::vector<RecommendedPoint>& recommendedPath,
                      const std::function<bool()>& shouldKeepRunning);

private:
    std::list<Cmd> emptyQueue();
    void processLobbyCmds();
    void broadcastTimeLeft(float remaining_sec);
    void broadcastRecommendedPath(const std::vector<RecommendedPoint>& recommendedPath);
    void broadcastUpgradeLogic();


    std::shared_ptr<gameLoopQueue> queue;
    ClientsRegistry& registry;
    PlayerManager& playerManager;
    std::unordered_map<ID, Car>& playerCars;
    const std::vector<UpgradeDef>& upgrades;
    const Config& config;

    bool& startRequested;
    bool& raceStarted;
    uint8_t& totalCars;

};

#endif