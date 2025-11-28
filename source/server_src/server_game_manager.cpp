#include "server_game_manager.h"

#include "../common_src/srv_msg/joingame.h"
#include "../common_src/srv_msg/metadatagames.h"
#include "game_logic/server_gameloop.h"

std::pair<std::shared_ptr<gameLoopQueue>, ID>
GameManager::CreateJoinGame(ID game_id, SendQPtr sender_queue, ID client_id) {
    if (!sender_queue) {
        throw std::runtime_error("sender_queue null");
    }

    if (game_id == 0) { // id estandar para crear partida
        auto reg = std::make_shared<ClientsRegistry>();
        auto q = std::make_shared<gameLoopQueue>();
        auto loop = std::make_unique<GameLoop>(q, reg);

        reg->AddClient(sender_queue, client_id);

        GameLoop* raw = loop.get();
        ID gid;
        {
            std::lock_guard<std::mutex> lk(mx);
            gid = ++last_id;
            games.emplace(gid, GameContext(reg, q, std::move(loop)));
            std::cout << "[Game Manager] New game created, game id: " << gid << std::endl;
        }

        sender_queue->push(std::make_shared<JoinGame>(true, SUCCES, gid));
        raw->start();
        std::cout << "[Game Manager] Player joined:  " << client_id << ", game id: " << static_cast<int>(last_id) << std::endl;
        return {q, gid};
    } else { // pedido de unirse a una partida existente
        std::shared_ptr<ClientsRegistry> reg;
        std::shared_ptr<gameLoopQueue> q;
        {
            std::lock_guard<std::mutex> lk(mx);
            auto it = games.find(game_id);
            if (it == games.end()) {
                sender_queue->push(std::make_shared<JoinGame>(false, INEXISTENT_GAME, 0));
                throw std::runtime_error("Partida inexistente");
            }
            if (it->second.isStarted()) {
                sender_queue->push(std::make_shared<JoinGame>(false, STARTED_GAME, 0));
                throw std::runtime_error("Partida inexistente");
            }
            reg = it->second.getRegistry();
            q   = it->second.getGameQueue();
        }
        try {
            reg->AddClient(sender_queue, client_id);
            sender_queue->push(std::make_shared<JoinGame>(true, SUCCES, game_id));
            std::cout << "[Game Manager] Player joined:  " << client_id << ", game id: " << game_id<< std::endl;
        } catch (const std::exception&) {
            sender_queue->push(std::make_shared<JoinGame>(false, FULL_GAME, 0));
            throw(std::runtime_error("Partida llena"));
        }
        return {q, game_id};
    }
}

void GameManager::LeaveGame(ID client_id, ID game_id) {
    std::unique_ptr<GameLoop> to_stop;
    {
        std::lock_guard<std::mutex> lk(mx);
        auto it = games.find(game_id);
        if (it == games.end()) return;
        it->second.getRegistry()->EraseQueue(client_id);
        std::cout << "[Game Manager] Player id: " << client_id << " left game: " << static_cast<uint32_t>(game_id) << std::endl;
        if (it->second.getRegistry()->size() == 0) {
            to_stop = it->second.takeGameThread();  // mover el loop
            games.erase(it);                       // borrar del mapa ahora que ya no hay hilos
        }
    }
    if (to_stop) {
        to_stop->stop();
        to_stop->join();
        std::cout << "[GameManager] Reaped empty game: " << game_id << std::endl;
    }

}

MetadataGames GameManager::getGames() {
    std::vector<GameMetadata> out;
    {
        std::lock_guard<std::mutex> lk(mx);
        out.reserve(games.size());
        for (auto& kv : games) {
            ID gid = kv.first;
            GameContext& ctx = kv.second;
            int players = ctx.getRegistry() ? ctx.getRegistry()->size() : 0;
            bool started = ctx.isStarted(); //por ahora hardcodeado pero desp tino decime de donde lo saco

            out.push_back(GameMetadata{gid, players, started});
        }
    }
    return {out};
}
