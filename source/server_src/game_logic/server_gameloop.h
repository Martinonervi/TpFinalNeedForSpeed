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
#include "chrono"

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

    void waitingForPlayers();

    // race loop
    void checkPlayersStatus();
    void processCmds();
    void broadcastCarSnapshots();
    void processWorldEvents();
    void sendCurrentInfo();

    // stats race
    void sendPlayerStats();

    //  processCmds()
    std::list<Cmd> emptyQueue();
    void movementHandler(Cmd& cmd);
    void initPlayerHandler(Cmd& cmd);

    void disconnectHandler(ID id);
    bool isConnected(ID id) const;

    void resetRaceState();
    void setupRoute();


    std::shared_ptr<gameLoopQueue> queue;
    std::shared_ptr<ClientsRegistry> registry;
    std::queue<WorldEvent> worldEvents;
    WorldEventHandlers eventHandlers;

    // box2D
    WorldManager worldManager;
    std::unordered_map<ID, Car> cars;
    std::unordered_map<ID,Checkpoint> checkpoints;
    std::vector<std::unique_ptr<Building>> buildings;
    std::vector<SpawnPointConfig> spawnPoints;

    // race flags
    float raceTimeSeconds = 0.0f;
    std::chrono::steady_clock::time_point raceStartTime;
    bool raceStarted = false;
    bool raceEnded   = false;
    uint8_t finishedCarsCount = 0;
    uint8_t totalCars = 0;
    uint8_t raceCarNumber = 0;
    std::vector<ID> raceRanking;

    MapData mapData;

    uint8_t raceIndex = 0;

    // loop
    Printer printer;
    void simulatePlayerSpawns(int numPlayers);
};

#endif
