#ifndef SERVER_GAMELOOP_H
#define SERVER_GAMELOOP_H

#include <list>
#include <map>
#include <unordered_set>

#include <box2d/box2d.h>

#include "../server_client_registry.h"
#include "../server_types.h"
#include "../world/entities/building.h"
#include "../world/entities/car.h"
#include "../world/entities/checkpoint.h"
#include "../world/map_parser.h"
#include "../world/world_manager.h"
#include "world_event_handlers.h"
#include <chrono>
#include "player_manager.h"



class GameLoop: public Thread {

public:
    GameLoop(std::shared_ptr<gameLoopQueue> queue, std::shared_ptr<ClientsRegistry> registry);
    void stop() override;
    virtual ~GameLoop();
    bool isRaceStarted() const;


protected:
    void run() override;

private:
    void loadMapFromYaml(const std::string& path);
    void runSingleRace();
    void setupRoute();
    void resetRaceState();
    void processLobbyCmds();
    void waitingForPlayers();

    // race loop
    void checkPlayersStatus();
    void processCmds();
    void processWorldEvents();
    void sendCurrentInfo();

    // others
    std::list<Cmd> emptyQueue();
    void disconnectHandler(ID id);
    bool isConnected(ID id) const;


    std::shared_ptr<gameLoopQueue> queue;
    std::shared_ptr<ClientsRegistry> registry;
    std::queue<WorldEvent> worldEvents;
    WorldEventHandlers eventHandlers;

    // box2D
    WorldManager worldManager;
    std::unordered_map<ID, Car> playerCars;
    std::vector<Car> npcParkedCars;
    std::unordered_map<ID,Checkpoint> checkpoints;
    std::vector<std::unique_ptr<Building>> buildings;
    std::vector<SpawnPointConfig> spawnPoints;
    std::vector<RecommendedPoint> recommendedPath;

    PlayerManager playerManager;

    // race flags
    float raceTimeSeconds = 0.0f;
    std::chrono::steady_clock::time_point raceStartTime;
    bool raceStarted = false;
    bool raceEnded   = false;
    uint8_t finishedCarsCount = 0;
    uint8_t totalCars = 0;
    std::vector<ID> raceRanking;
    uint8_t totalRaces = 0;
    MapData mapData;


    bool startRequested = false;
    uint8_t raceIndex = 0;


    Printer printer;
    void simulatePlayerSpawns(int numPlayers);


    std::vector<UpgradeDef> upgrades = {
            { Upgrade::ENGINE_FORCE, 2.5f, 8.0f },
            { Upgrade::HEALTH, 1.5f, 5.0f },
            { Upgrade::SHIELD, 0.7f, 2.0f },
            { Upgrade::DAMAGE, 2.f, 3.0f },
    };



    const UpgradeDef& findUpgradeDef(Upgrade type) const {
        for (const auto& u : upgrades) {
            if (u.type == type)
                return u;
        }
        throw std::runtime_error("UpgradeType desconocido");
    }

};



#endif
