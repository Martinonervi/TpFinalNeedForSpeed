//
// Created by Felipe Fialayre on 03/11/2025.
//

#include "server_game_context.h"

GameContext::GameContext(ClientsRegistry& registry_ref, gameLoopQueue& game_queue,
                         std::unique_ptr<GameLoop> game_thread):
        registry(registry_ref),
        queue(game_queue),
        game_thread(std::move(game_thread)) {}


ClientsRegistry& GameContext::getRegistry() {
    return registry;
}
gameLoopQueue& GameContext::getGameQueue() {
    return queue;
}
void GameContext::gameThreadStop() {
    game_thread->stop();
}
