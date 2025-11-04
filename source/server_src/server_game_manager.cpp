#include "server_game_manager.h"

#include "server_gameloop.h"

std::shared_ptr<gameLoopQueue> GameManager::CreateJoinGame(ID game_id, SendQPtr sender_queue, ID client_id) {
    std::lock_guard<std::mutex> lk(mx);

    if (game_id == 0) {

        auto new_clients_registry = std::make_shared<ClientsRegistry>();
        auto new_queue            = std::make_shared<gameLoopQueue>();
        auto game_thread_ptr      = std::make_unique<GameLoop>(new_queue, new_clients_registry);

        new_clients_registry->AddClient(sender_queue, client_id);
        game_thread_ptr->start();
        ID new_game_id = ++last_id;
        games.emplace(new_game_id, GameContext(new_clients_registry, new_queue, std::move(game_thread_ptr)));

        std::cout << "[GameManager] New game created, Game ID: " << last_id << std::endl;
        std::cout << "[GameManager] Player ID: " << client_id << " joined game: "<< new_game_id << std::endl;

        return new_queue;
    } else {
        auto it = games.find(game_id);
        if (it == games.end()) {
            throw("Partida inexistente"); // en realidad esto no va a pasar porque se manejará desde QT
        }
        it->second.getRegistry()->AddClient(std::move(sender_queue), client_id);
        std::cout << "[GameManager] Player ID: " << client_id << " joined game: "<< game_id << std::endl;
        return it->second.getGameQueue();
    }
}
