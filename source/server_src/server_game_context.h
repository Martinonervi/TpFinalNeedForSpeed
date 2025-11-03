//
// Created by Felipe Fialayre on 03/11/2025.
//

#ifndef SERVER_GAME_CONTEXT_H
#define SERVER_GAME_CONTEXT_H

#include "server_types.h"
#include "server_gameloop.h"


class GameContext {
public:
    GameContext(ClientsRegistry& registry_ref, gameLoopQueue& game_queue, std::unique_ptr<GameLoop> game_thread);

    ClientsRegistry& getRegistry();
    gameLoopQueue& getGameQueue();
    void gameThreadStop();

private:
    ClientsRegistry& registry;
    gameLoopQueue& queue;
    std::unique_ptr<GameLoop> game_thread;
};



#endif //SERVER_GAME_CONTEXT_H
