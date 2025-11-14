#ifndef SERVER_GAMELOOP_H
#define SERVER_GAMELOOP_H

#include <list>
#include <map>
#include "car.h"
#include "server_client_registry.h"
#include "server_types.h"
#include <box2d/box2d.h>
#include "world_manager.h"
#include "map_parser.h"
#include "checkpoint.h"
#include "building.h"
#include <unordered_set>

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
    void processCmds();

    std::list<Cmd> emptyQueue();

    void movementHandler(Cmd& cmd);
    void initPlayerHandler(Cmd& cmd);
    void disconnectHandler(ID id);
    void broadcastCarSnapshots();

    void CarHitCheckpointHandler(WorldEvent ev);
    void CarHitBuildingHandler(WorldEvent ev, std::unordered_set<ID>& alreadyHitBuildingThisFrame);
    void CarHitCarHandler(WorldEvent ev, std::unordered_set<ID>& alreadyHitCarPairThisFrame);


    // box2D
    WorldManager worldManager;
    std::unordered_map<ID, Car> cars;
    std::unordered_map<ID,Checkpoint> checkpoints;
    std::vector<std::unique_ptr<Building>> buildings;

    std::shared_ptr<gameLoopQueue> queue;
    std::shared_ptr<ClientsRegistry> registry;
    std::queue<WorldEvent> worldEvents;
    void processWorldEvents();

    void checkPlayersStatus();
    void waitingForPlayers();

    //tiempo de la carrera
    float raceTimeSeconds = 0.0f;
    bool raceStarted = false;
    bool raceEnded   = false;
    int finishedCarsCount = 0;
    int totalCars = 0;

    // loop
    Printer printer;
};

#endif
