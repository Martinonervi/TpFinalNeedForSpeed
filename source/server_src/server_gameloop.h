#pragma once

#include <list>
#include <map>
#include "car.h"
#include "server_client_registry.h"
#include "server_types.h"
#include <box2d/box2d.h>

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
    void broadcastCarSnapshots();

    b2WorldId world;
    std::map<ID, Car> cars;

    std::shared_ptr<gameLoopQueue> queue;
    std::shared_ptr<ClientsRegistry> registry;
    Printer printer;
};


