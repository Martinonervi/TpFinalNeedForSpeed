#include "player_manager.h"

// player_manager.cpp
#include "../../common_src/cli_msg/cli_cheat_request.h"
#include "../../common_src/srv_msg/srv_car_select.h"

#include "player_manager.h"

PlayerManager::PlayerManager(WorldManager& world,
                             ClientsRegistry& registry,
                             std::unordered_map<ID, Car>& playerCars,
                             const std::vector<SpawnPointConfig>& spawnPoints, bool& raceStarted,
                             const std::unordered_map<ID,Checkpoint>& checkpoints,
                             const Config& config):
    world(world), registry(registry),
    playerCars(playerCars), spawnPoints(spawnPoints),
    raceStarted(raceStarted), checkpoints(checkpoints), config(config)
{}

bool PlayerManager::initPlayer(Cmd& cmd) {
    auto init_msg = std::static_pointer_cast<SrvMsg>(
        std::make_shared<CarSelect>(!raceStarted));
    registry.sendTo(cmd.client_id, init_msg);
    if (raceStarted) {
        std::cout << "[Player Manager] ya empezó"<< std::endl;
        return false;
    }
    const auto& ip  = dynamic_cast<const InitPlayer&>(*cmd.msg);

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
    playerCars.emplace(cmd.client_id,
                 Car(this->world, cmd.client_id, spawnVec, spawn.angle,
                     ip.getCarType(), config.carHandling));

    // le aviso al cliente que ya tiene su auto
    auto base = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SendPlayer>(cmd.client_id,
                                         ip.getCarType(),
                                         spawn.x, spawn.y,
                                         spawn.angle));
    registry.sendTo(cmd.client_id, base);

    // le aviso al nuevo cliente dónde están los otros autos
    for (auto [id, car] : playerCars) {
        if (id == cmd.client_id) continue;

        auto posCar = car.getPosition();
        auto newPlayer = std::static_pointer_cast<SrvMsg>(
                std::make_shared<NewPlayer>(id,
                                            car.getCarType(),
                                            posCar.x, posCar.y, car.getAngleRad()));
        registry.sendTo(cmd.client_id, newPlayer);
    }

    // les aviso a todos del auto del nuevo cliente
    for (auto& [otherId, _] : playerCars) {
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

void PlayerManager::cheatHandler(Cmd& cmd) {
    const auto& cheatRequest  = dynamic_cast<const CheatRequest&>(*cmd.msg);
    const auto cheat = cheatRequest.getCheat();

    auto it = playerCars.find(cmd.client_id);
    if (it == playerCars.end()) return;
    Car& car = it->second;

    switch (cheat) {
        case (Cheat::HEALTH_CHEAT):
            if (!config.cheats.allowHealthCheat) return;;
            car.applyCheat(cheatRequest.getCheat());
            break;
        case (Cheat::FREE_SPEED_CHEAT): {
            if (!config.cheats.allowFreeSpeedCheat) return;;
            car.applyCheat(cheatRequest.getCheat());
            break;
        }
        case (Cheat::NEXT_CHECKPOINT_CHEAT): {
            if (!config.cheats.allowNextCheckpointCheat) return;
            const auto& actualCheckpoint = car.getActualCheckpoint();
            auto it = checkpoints.find(actualCheckpoint + 1);
            if (it == checkpoints.end()) return;
            auto newCheckpoint = it->second;
            car.setPosition(newCheckpoint.getX(), newCheckpoint.getY());
            break;
        }

        default: {
            std::cout << "[Gameloop] comando desconocido: " << static_cast<int>(cmd.msg->type()) << "\n";
        }
    }

}

void PlayerManager::handleMovement(Cmd& cmd, float dt) {
    auto it = playerCars.find(cmd.client_id);
    if (it == playerCars.end()) return;
    if (it->second.isCarDestroy()) return;

    const auto& mv = dynamic_cast<const MoveMsg&>(*cmd.msg);
    it->second.applyControlsToBody(mv, dt);
}

void PlayerManager::disconnectPlayer(ID id) {
    auto it = playerCars.find(id);
    if (it == playerCars.end()) return;

    // libero spawn si lo tenía
    auto itSpawn = carToSpawnId.find(id);
    if (itSpawn != carToSpawnId.end()) {
        usedSpawnIds.erase(itSpawn->second);
        carToSpawnId.erase(itSpawn);
    }

    // destruyo el cuerpo físico
    world.destroyEntity(it->second.getPhysicsId());

    // borro el auto
    playerCars.erase(it);
}

void PlayerManager::broadcastSnapshots() {
    for (auto& [id, car] : playerCars) {
        PlayerState ps = car.snapshotState();
        auto base = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerState>(std::move(ps)));
        registry.broadcast(base);
    }
}

void PlayerManager::sendPlayerStats(
        const std::unordered_map<ID, PlayerGlobalStats>& globalStats
) {
    std::cout << "[DBG] globalStats size = " << globalStats.size() << "\n";
    for (auto& [id, stats] : globalStats) {
        std::cout << "  playerId=" << id
                  << " totalTime=" << stats.totalTime
                  << " globalPos=" << (int)stats.globalPosition << "\n";
    }

    
    for (auto& [id, car] : playerCars) {
        float   totalTime   = 0.0f;
        uint8_t globalPos   = 0;    // 0 = sin ranking (nunca terminó)

        auto it = globalStats.find(id);
        if (it != globalStats.end()) {
            totalTime = it->second.totalTime;
            globalPos = it->second.globalPosition;
        }

        // PlayerStats(racePosition, timeSecToComplete)
        PlayerStats ps(globalPos, totalTime);

        auto msg = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerStats>(std::move(ps)));
        registry.sendTo(id, msg);
    }
}


void PlayerManager::resetForNewRace() {
    usedSpawnIds.clear();
    carToSpawnId.clear();
}
