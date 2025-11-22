#include "player_manager.h"

// player_manager.cpp
#include "player_manager.h"

PlayerManager::PlayerManager(WorldManager& world,
                             ClientsRegistry& registry,
                             std::unordered_map<ID, Car>& cars,
                             const std::vector<SpawnPointConfig>& spawnPoints)
        : world(world)
          , registry(registry)
          , cars(cars)
          , spawnPoints(spawnPoints)
{}

bool PlayerManager::initPlayer(Cmd& cmd) {
    const auto& ip = dynamic_cast<const InitPlayer&>(*cmd.msg);

    if (spawnPoints.empty()) {
        std::cerr << "[PlayerManager] ERROR: no hay spawnPoints cargados\n";
        return false;
    }

    // buscar spawn libre
    int chosenIndex = -1;
    for (int i = 0; i < (int)spawnPoints.size(); ++i) {
        const auto& sp = spawnPoints[i];
        if (!usedSpawnIds.count(sp.spawnId)) {
            chosenIndex = i;
            break;
        }
    }

    if (chosenIndex == -1) {
        std::cerr << "[PlayerManager] WARNING: no hay spawn libre\n";
        return false;
    }

    const auto& spawn = spawnPoints[chosenIndex];

    // marco el spawn como usado
    usedSpawnIds.insert(spawn.spawnId);
    carToSpawnId[cmd.client_id] = spawn.spawnId;

    // creo el auto
    b2Vec2 spawnVec = { spawn.x, spawn.y };
    cars.emplace(cmd.client_id,
                 Car(this->world, cmd.client_id, spawnVec, spawn.angle,
                     ip.getCarType()));

    // le aviso al cliente que ya tiene su auto
    auto base = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SendPlayer>(cmd.client_id,
                                         ip.getCarType(),
                                         spawn.x, spawn.y,
                                         3 /* placeholder lap o algo */));
    registry.sendTo(cmd.client_id, base);

    // le aviso al nuevo cliente dónde están los otros autos
    for (auto [id, car] : cars) {
        if (id == cmd.client_id) continue;

        auto newPlayer = std::static_pointer_cast<SrvMsg>(
                std::make_shared<NewPlayer>(id,
                                            car.getCarType(),
                                            1, 2, 3)); // TODO: pos reales
        registry.sendTo(cmd.client_id, newPlayer);
    }

    // les aviso a todos del auto del nuevo cliente
    for (auto& [otherId, _] : cars) {
        if (otherId == cmd.client_id) continue;

        auto npForOld = std::static_pointer_cast<SrvMsg>(
                std::make_shared<NewPlayer>(cmd.client_id,
                                            ip.getCarType(),
                                            spawn.x, spawn.y,
                                            spawn.angle));
        registry.sendTo(otherId, npForOld);
    }

    return true;
}

void PlayerManager::handleMovement(Cmd& cmd, float dt) {
    auto it = cars.find(cmd.client_id);
    if (it == cars.end()) return;
    if (it->second.isCarDestroy()) return;

    const auto& mv = dynamic_cast<const MoveMsg&>(*cmd.msg);
    it->second.applyControlsToBody(mv, dt);
}

void PlayerManager::disconnectPlayer(ID id) {
    auto it = cars.find(id);
    if (it == cars.end()) return;

    // libero spawn si lo tenía
    auto itSpawn = carToSpawnId.find(id);
    if (itSpawn != carToSpawnId.end()) {
        usedSpawnIds.erase(itSpawn->second);
        carToSpawnId.erase(itSpawn);
    }

    // destruyo el cuerpo físico
    world.destroyEntity(it->second.getPhysicsId());

    // borro el auto
    cars.erase(it);
}

void PlayerManager::broadcastSnapshots() {
    for (auto& [id, car] : cars) {
        PlayerState ps = car.snapshotState();
        auto base = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerState>(std::move(ps)));
        registry.broadcast(base);
    }
}

void PlayerManager::sendPlayerStats() {
    for (auto& [id, car] : cars) {
        PlayerStats ps(car.getRanking(), car.getFinishTime());
        auto msg = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerStats>(std::move(ps)));
        registry.sendTo(id, msg);
    }
}

void PlayerManager::resetForNewRace() {
    usedSpawnIds.clear();
    carToSpawnId.clear();
}
