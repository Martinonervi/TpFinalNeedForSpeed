#include "player_manager.h"
#include "../../common_src/cli_msg/cli_cheat_request.h"
#include "../../common_src/srv_msg/srv_car_select.h"


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

    int chosenIndex = findFreeSpawnIndex();

    if (chosenIndex == -1) {
        std::cerr << "[PlayerManager] WARNING: no hay spawn libre\n";
        return false;
    }

    const auto& spawn = spawnPoints[chosenIndex];

    createCarForClient(cmd.client_id, ip, spawn); //le mando al cliente su auto
    notifyClientExistingCars(cmd.client_id); //le mando al cliente los otros autos
    notifyOthersAboutNewCar(cmd.client_id, ip.getCarType(), spawn); //mando a todos el auto nuevo
    return true;
}

int PlayerManager::findFreeSpawnIndex() const {
    for (int i = 0; i < static_cast<int>(spawnPoints.size()); ++i) {
        const auto& sp = spawnPoints[i];
        if (!usedSpawnIds.count(sp.spawnId)) {
            return i;
        }
    }
    return -1;
}

void PlayerManager::createCarForClient(ID clientId, const InitPlayer& ip,
                                       const SpawnPointConfig& spawn) {
    // marco el spawn como usado
    usedSpawnIds.insert(spawn.spawnId);
    carToSpawnId[clientId] = spawn.spawnId;

    // creo el auto
    b2Vec2 spawnVec{ spawn.x, spawn.y };
    playerCars.emplace(
        clientId,
        Car(world, clientId, spawnVec, spawn.angle,
            ip.getCarType(), config.carHandling)
    );

    // le aviso al cliente que ya tiene su auto
    auto msg = std::static_pointer_cast<SrvMsg>(
        std::make_shared<SendPlayer>(
            clientId,
            ip.getCarType(),
            spawn.x, spawn.y,
            spawn.angle
        )
    );
    registry.sendTo(clientId, msg);
}

void PlayerManager::notifyClientExistingCars(ID newClientId) {
    for (const auto& [id, car] : playerCars) {
        if (id == newClientId) continue;

        auto posCar = car.getPosition();
        auto newPlayerMsg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<NewPlayer>(
                id,
                car.getCarType(),
                posCar.x, posCar.y,
                car.getAngleRad()
            )
        );
        registry.sendTo(newClientId, newPlayerMsg);
    }
}

void PlayerManager::handleMovement(Cmd& cmd, float dt) {
    auto it = playerCars.find(cmd.client_id);
    if (it == playerCars.end()) return;
    if (it->second.isCarDestroy()) return;

    const auto& mv = dynamic_cast<const MoveMsg&>(*cmd.msg);
    it->second.applyControlsToBody(mv, dt);
}

void PlayerManager::notifyOthersAboutNewCar(ID newClientId,
                                            CarType carType,
                                            const SpawnPointConfig& spawn) {
    for (auto& [otherId, _] : playerCars) {
        if (otherId == newClientId) continue;

        auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<NewPlayer>(
                newClientId,
                carType,
                spawn.x, spawn.y,
                spawn.angle
            )
        );
        registry.sendTo(otherId, msg);
    }
}

void PlayerManager::cheatHandler(Cmd& cmd) {
    const auto& cheatRequest  = dynamic_cast<const CheatRequest&>(*cmd.msg);
    const auto cheat = cheatRequest.getCheat();
    auto it = playerCars.find(cmd.client_id);
    if (it == playerCars.end()) return;
    Car& car = it->second;

    switch (cheat) {
        case (Cheat::HEALTH_CHEAT):
            if (!config.cheats.allowHealthCheat) return;
            car.applyCheat(cheatRequest.getCheat());
            break;
        case (Cheat::FREE_SPEED_CHEAT): {
            if (!config.cheats.allowFreeSpeedCheat) return;
            car.applyCheat(cheatRequest.getCheat());
            break;
        }
        case (Cheat::NEXT_CHECKPOINT_CHEAT): {
            if (!config.cheats.allowNextCheckpointCheat) return;
            const auto& actualCheckpoint = car.getActualCheckpoint();
            auto itCp = checkpoints.find(actualCheckpoint + 1);
            if (itCp == checkpoints.end()) return;
            auto newCheckpoint = itCp->second;
            car.setPosition(newCheckpoint.getX(), newCheckpoint.getY());
            break;
        }
        default: {
            std::cout << "[PlayerManager] comando desconocido: " << static_cast<int>(cmd.msg->type()) << "\n";
        }
    }
}

void PlayerManager::broadcastSnapshots() {
    for (auto& [id, car] : playerCars) {
        PlayerState ps = car.snapshotState();
        auto base = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerState>(std::move(ps)));
        registry.broadcast(base);
    }
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

void PlayerManager::sendPlayerStats(const std::unordered_map<ID, PlayerGlobalStats>& globalStats) {
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
        PlayerStats ps(globalPos, totalTime);

        auto msg = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerStats>(std::move(ps)));
        registry.sendTo(id, msg);
    }
}

