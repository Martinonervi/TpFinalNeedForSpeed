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

class GameLoop: public Thread {

public:
    GameLoop(std::shared_ptr<gameLoopQueue> queue, std::shared_ptr<ClientsRegistry> registry);
    void stop() override;
    virtual ~GameLoop();

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

    // box2D
    WorldManager worldManager;
    std::unordered_map<ID, Car> cars;
    std::vector<std::unique_ptr<Checkpoint>> checkpoints;
    std::vector<std::unique_ptr<Building>> buildings;
    std::unordered_map<ID, MoveMsg> lastInput;

    std::shared_ptr<gameLoopQueue> queue;
    std::shared_ptr<ClientsRegistry> registry;
    std::queue<WorldEvent> worldEvents;
    void processWorldEvents();

    // loop
    Printer printer;
};

#endif
