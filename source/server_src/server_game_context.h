//
// Created by Felipe Fialayre on 03/11/2025.
//

#ifndef SERVER_GAME_CONTEXT_H
#define SERVER_GAME_CONTEXT_H

#include "game_logic/server_gameloop.h"

#include "server_types.h"


class GameContext {
public:
    GameContext(std::shared_ptr<ClientsRegistry> registry_ref, std::shared_ptr<gameLoopQueue> game_queue, std::unique_ptr<GameLoop> game_thread);

    std::shared_ptr<ClientsRegistry> getRegistry();
    std::shared_ptr<gameLoopQueue> getGameQueue();
    void gameThreadStop();
    std::unique_ptr<GameLoop> takeGameThread();
    bool isStarted() const;

private:
    std::shared_ptr<ClientsRegistry> registry;
    std::shared_ptr<gameLoopQueue> queue;
    std::unique_ptr<GameLoop> game_thread;
};



#endif //SERVER_GAME_CONTEXT_H
