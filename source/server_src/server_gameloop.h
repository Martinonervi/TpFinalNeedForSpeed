#pragma once

#include <list>
#include <map>
#include "car.h"
#include "server_client_registry.h"
#include "server_types.h"
#include <box2d/box2d.h>
#include "world_manager.h"

class GameLoop: public Thread {

public:
    GameLoop(std::shared_ptr<gameLoopQueue> queue, std::shared_ptr<ClientsRegistry> registry);
    void stop() override;
    virtual ~GameLoop();

protected:
    void run() override;

private:
    void processCmds();

    std::list<Cmd> emptyQueue();

    void movementHandler(Cmd& cmd);
    void initPlayerHandler(Cmd& cmd);
    void disconnectHandler(ID id);
    void broadcastCarSnapshots();

    WorldManager worldManager;
    std::unordered_map<ID, Car> cars;
    std::unordered_map<ID, EntityId> clientToEntity;
    std::unordered_map<ID, MoveMsg> lastInput;

    std::shared_ptr<gameLoopQueue> queue;
    std::shared_ptr<ClientsRegistry> registry;
    Printer printer;
};


