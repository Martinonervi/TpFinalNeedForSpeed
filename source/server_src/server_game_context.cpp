//
// Created by Felipe Fialayre on 03/11/2025.
//

#include "server_game_context.h"

GameContext::GameContext(std::shared_ptr<ClientsRegistry> registry, std::shared_ptr<gameLoopQueue> game_queue,
                         std::unique_ptr<GameLoop> game_thread):
        registry(std::move(registry)),
        queue(std::move(game_queue)),
        game_thread(std::move(game_thread)) {}


std::shared_ptr<ClientsRegistry> GameContext::getRegistry() {
    return registry;
}
std::shared_ptr<gameLoopQueue> GameContext::getGameQueue() {
    return queue;
}

std::unique_ptr<GameLoop> GameContext::takeGameThread() {
    return std::move(game_thread);
}

bool GameContext::isStarted() const {
    return game_thread->isRaceStarted();
}

