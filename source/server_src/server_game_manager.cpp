#include "server_game_manager.h"

#include "../common_src/joingame.h"

#include "server_gameloop.h"

std::pair<std::shared_ptr<gameLoopQueue>, ID> GameManager::CreateJoinGame(ID game_id, SendQPtr sender_queue, ID client_id) {
    std::lock_guard<std::mutex> lk(mx);

    std::shared_ptr<gameLoopQueue> queue;
    ID joined_game_id;

    if (game_id == 0) {

        auto new_clients_registry = std::make_shared<ClientsRegistry>();
        queue = std::make_shared<gameLoopQueue>();
        auto game_thread_ptr = std::make_unique<GameLoop>(queue, new_clients_registry);
        try {
            new_clients_registry->AddClient(sender_queue, client_id);
        } catch (const std::exception& e) {
            std::shared_ptr<JoinGame> msg = std::make_shared<JoinGame>(false, UNKNOWN_ERR);
            sender_queue->push(msg);
            throw(std::runtime_error("Error creando partida"));
        }

        std::shared_ptr<JoinGame> msg = std::make_shared<JoinGame>(true, SUCCES);
        sender_queue->push(msg);

        game_thread_ptr->start();
        joined_game_id = ++last_id;
        games.emplace(joined_game_id, GameContext(new_clients_registry, queue, std::move(game_thread_ptr)));

        std::cout << "[GameManager] New game created, Game ID: " << last_id << std::endl;
        std::cout << "[GameManager] Player ID: " << client_id << " joined game: "<< joined_game_id << std::endl;

    } else {
        auto it = games.find(game_id);
        if (it == games.end()) {
            std::shared_ptr<JoinGame> msg = std::make_shared<JoinGame>(false, INEXISTENT_GAME);
            sender_queue->push(msg);
            throw(std::runtime_error("Partida inexistente"));
        }
        joined_game_id = game_id;
        try {
            it->second.getRegistry()->AddClient(std::move(sender_queue), client_id);
        }catch (const std::exception& e) {
                std::shared_ptr<JoinGame> msg = std::make_shared<JoinGame>(false, FULL_GAME);
                sender_queue->push(msg);
                throw(std::runtime_error("Partida llena"));
        }
        std::cout << "[GameManager] Player ID: " << client_id << " joined game: "<< game_id << std::endl;
        queue = it->second.getGameQueue();
    }
    //reap_dead();
    if (queue) return {queue, joined_game_id};
    throw std::runtime_error("Error obteniendo queue");
}
/*
void GameManager::reap_dead() {
    for (auto it = games.begin(); it != games.end(); ) {
        if (it->second.getRegistry()->size() == 0) {
            it->second.gameThreadStop();
            it = games.erase(it);
        } else {
            ++it; // solo avanza si no borro
        }
    }
}
*/
void GameManager::LeaveGame(ID client_id, ID game_id) {
    std::lock_guard<std::mutex> lk(mx);
    auto it = games.find(game_id);
    if (it == games.end()) return;

    it->second.getRegistry()->EraseQueue(client_id);

    // si quedó vacía, cerramos hilo y limpiamos
    if (it->second.getRegistry()->size() == 0) {
        it->second.gameThreadStop();  // stop + join
        games.erase(it);
        std::cout << "[GameManager] Reaped empty game: " << game_id << std::endl;
    }
}
