#include "server_game_manager.h"

#include "server_gameloop.h"

std::pair<gameLoopQueue&, ID> GameManager::CreateJoinGame(ID game_id, SendQPtr sender_queue) {
    std::lock_guard<std::mutex> lk(mx);

    if (game_id == 0) {
        ClientsRegistry new_clients_registry = ClientsRegistry();
        gameLoopQueue new_queue;
        auto game_thread_ptr = std::make_unique<GameLoop>(new_queue, new_clients_registry);
        game_thread_ptr->start();

        // ACA FALTA LA LINEA DE AGREGAR AL PRIMER CLIENTE AL REGISTRY PERO TENGO Q CAMBIAR LA IMPLEMENTACION
        ID ingame_id = new_clients_registry.AddClient(sender_queue);
        GameContext new_context = GameContext(new_clients_registry, new_queue, std::move(game_thread_ptr));

        games.emplace(last_id, std::move(new_context));

        return {new_queue, ingame_id};
    } else {
        auto it = games.find(game_id);
        if (it == games.end()) {
            throw("Partida inexistente"); // en realidad esto no va a pasar porque se maneja desde QT
        }
        ID ingame_id = it->second.getRegistry().AddClient(std::move(sender_queue));
        return { it->second.getGameQueue(), ingame_id };
    }
}
