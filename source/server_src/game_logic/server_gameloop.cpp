#include "server_gameloop.h"
#include <chrono>
#include <thread>
#include <cmath>
#include "../../common_src/cli_msg/init_player.h"
#include "../../common_src/cli_msg/move_Info.h"
#include "../../common_src/constant_rate_loop.h"
#include "../../common_src/srv_msg/client_disconnect.h"
#include "../../common_src/srv_msg/new_player.h"
#include "../../common_src/srv_msg/player.h"
#include "../../common_src/srv_msg/player_state.h"
#include "../../common_src/srv_msg/send_player.h"
#include "../../common_src/srv_msg/srv_car_hit_msg.h"
#include "../../common_src/srv_msg/srv_checkpoint_hit_msg.h"
#include "../../common_src/srv_msg/srv_current_info.h"
#include "../../common_src/srv_msg/playerstats.h"
#include "../../common_src/srv_msg/srv_time_left.h"
#include "../../common_src/srv_msg/srv_send_upgrade.h"
#include "../../common_src/cli_msg/cli_request_upgrade.h"

#define TIME_STEP 1.0f / 60.0f //cuánto tiempo avanza el mundo en esa llamada.
#define SUB_STEP_COUNT 4 //por cada timeStep resuelve problemas 4 veces mas rapido (ej: colisiones)
#define FILE_YAML_PATH "../server_src/world/map.yaml"
using Clock = std::chrono::steady_clock;

GameLoop::GameLoop(std::shared_ptr<gameLoopQueue> queue, std::shared_ptr<ClientsRegistry> registry):
worldEvents(), worldManager(worldEvents),queue(std::move(queue)),
registry(std::move(registry)), eventHandlers(cars, checkpoints, *this->registry,
            raceTimeSeconds, finishedCarsCount, totalCars, raceEnded, raceRanking),
        playerManager(worldManager, *this->registry, cars, spawnPoints)  {
    loadMapFromYaml(FILE_YAML_PATH);
}


// crea mapa con edificios y guarda la demas info
// parsea todos los recorridos del yaml
void GameLoop::loadMapFromYaml(const std::string& path) {
    MapParser parser;
    MapData data = parser.load(path);

    for (const auto& b : data.buildings) {
        auto building = std::make_unique<Building>(
                worldManager,
                b.x, b.y,
                b.w, b.h,
                b.angle
        );
        buildings.push_back(std::move(building));
    }
    this->mapData = data;
}

// loop principal de distinas carreras
void GameLoop::run() {
    setupRoute();
    const int racesToPlay = 1;
    this->totalRaces = mapData.routes.size(); // seria el raceToPlay,
                                              // se lo paso para ver si le llega bien a fran
    //const int racesToPlay = mapData.routes.size();

    while (raceIndex < racesToPlay && should_keep_running()) {
        waitingForPlayers();
        runSingleRace();
        //storeRaceResult(raceIndex);

        raceIndex += 1;
        if (raceIndex < racesToPlay) { //no termino la ultima
            resetRaceState();
        }
    }

    playerManager.sendPlayerStats();
}

//setea el proximo recorrido
void GameLoop::setupRoute() {
    auto routeIndex = raceIndex;
    if (mapData.routes.empty()) {
        std::cerr << "[GameLoop] setupRoute: el mapa no tiene rutas definidas\n";
        return;
    }
    if (routeIndex < 0 || routeIndex >= static_cast<int>(mapData.routes.size())) {
        std::cerr << "[GameLoop] setupRoute: routeIndex fuera de rango: "
                  << routeIndex << " (hay " << mapData.routes.size() << " rutas)\n";
        return;
    }

    const RouteConfig& routeCfg = mapData.routes[routeIndex];

    checkpoints.clear();
    spawnPoints.clear();

    for (const auto& cpCfg : routeCfg.checkpoints) {
        checkpoints.emplace(
                cpCfg.id,
                Checkpoint(
                        worldManager,
                        cpCfg.id,
                        cpCfg.kind,
                        cpCfg.x, cpCfg.y,
                        cpCfg.w, cpCfg.h,
                        cpCfg.angle
                        )
        );
    }

    spawnPoints = routeCfg.spawnPoints;

    std::cout << "[GameLoop] setupRoute: usando ruta '"
              << routeCfg.nameRoute
              << "' (index=" << routeIndex
              << "), checkpoints=" << checkpoints.size()
              << ", spawns=" << spawnPoints.size()
              << "\n";
}

void GameLoop::waitingForPlayers() {
    ConstantRateLoop loop(5.0);
    const int MAX_PLAYERS = 8;
    const double LOBBY_TIMEOUT_SEC = 5.0;
    const double BETWEEN_RACES_SEC    = 3.0;

    startRequested = false;
    raceStarted    = false;

    auto start = Clock::now();
    double timeout = (raceIndex == 0) ? LOBBY_TIMEOUT_SEC
                                        : BETWEEN_RACES_SEC;

    const auto deadline = start + std::chrono::duration<double>(timeout);
    while (should_keep_running()) {
        processLobbyCmds();

        if (startRequested) break;
        //if (raceIndex == 0 && registry->size() >= MAX_PLAYERS) break;
        if (Clock::now() >= deadline) break;

        auto now = Clock::now();
        float remaining_sec = std::chrono::duration_cast<std::chrono::duration<float>>(
                                      deadline - now
                                      ).count();

        if (remaining_sec < 0.f) remaining_sec = 0.f;


        // (2.9 → 2, 2.1 → 2, 1.9 → 1)
        uint8_t timeToSend = static_cast<uint8_t>(std::floor(remaining_sec));

        auto msg = std::static_pointer_cast<SrvMsg>(
                std::make_shared<TimeLeft>(timeToSend));
        registry->broadcast(msg);

        loop.sleep_until_next_frame();
    }
    this->raceStarted = true;
}

void GameLoop::resetRaceState() {
    worldManager.resetCheckpoints(checkpoints);
    setupRoute();

    uint8_t i = 0;
    for (auto& [id, car] : cars) {
        const auto& sp = spawnPoints[i % spawnPoints.size()];
        car.resetForNewRace(sp.x, sp.y, sp.angle);
        i++;
    }

    // reseteo variables
    raceTimeSeconds    = 0.f;
    finishedCarsCount  = 0;
    totalCars          = static_cast<int>(cars.size());
    raceEnded          = false;
    raceRanking.clear();

    // vaciar queues
    while (!worldEvents.empty()) {
        worldEvents.pop();
    }
    if (raceIndex > 0) {
        Cmd cmd_aux;
        while (queue->try_pop(cmd_aux)) {}
    }
}



void GameLoop::runSingleRace() {
    raceStartTime = Clock::now();
    try {
        ConstantRateLoop loop(60.0);

        while (should_keep_running()) {
            checkPlayersStatus();
            processCmds();
            worldManager.step(TIME_STEP, SUB_STEP_COUNT);

            if (!raceEnded) {
                auto now = Clock::now();
                std::chrono::duration<float> elapsed = now - raceStartTime;
                raceTimeSeconds = elapsed.count();

                const float MAX_RACE_TIME_SECONDS = 300.0f;
                if (raceTimeSeconds >= MAX_RACE_TIME_SECONDS) {
                    this->raceEnded = true;
                    // asignar ranking?
                }
            }

            processWorldEvents();
            if (raceEnded) break;
            playerManager.broadcastSnapshots();
            sendCurrentInfo();
            loop.sleep_until_next_frame();
        }

    } catch (const std::exception& e) {
        std::cerr << "[GameLoop] fatal: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[GameLoop] fatal: unknown\n";
    }
}


void GameLoop::checkPlayersStatus() {
    std::vector<ID> ids;
    for (auto& car: cars) {
        ids.push_back(car.first);
    }
    std::vector<ID> toDisconnect = registry->checkClients(ids);
    for (ID idToDisconnect : toDisconnect) {
        disconnectHandler(idToDisconnect);
        std::cout << "[GameLoop] auto con id: " << idToDisconnect
        << " borrado" << "\n";
        auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<ClientDisconnect>(idToDisconnect));
        registry->broadcast(msg);
    }
}

bool GameLoop::isRaceStarted() const {
    return this->raceStarted;
}

bool GameLoop::isConnected(ID id) const {
    return registry->contains(id);
}

void GameLoop::sendCurrentInfo() {
    for (auto& [id, car] : cars) {
        ID actual = car.getActualCheckpoint();
        ID next   = actual + 1;

        auto itCp = checkpoints.find(next);
        if (itCp == checkpoints.end()) continue; //termino
        const Checkpoint& cp = itCp->second;

        b2BodyId body = car.getBody();
        b2Vec2 pos = b2Body_GetPosition(body);

        float vx = cp.getX() - pos.x;
        float vy = cp.getY() - pos.y;

        float len = std::sqrt(vx*vx + vy*vy); //distancia del auto al checkpoint
        float angle = std::atan2(vy, vx);

        b2Vec2 vecVel = b2Body_GetLinearVelocity(body);
        float speed = std::sqrt(vecVel.x*vecVel.x + vecVel.y*vecVel.y);

        SrvCurrentInfo ci(cp.getId(), cp.getX(), cp.getY(), angle, len,
                          raceTimeSeconds, raceIndex+1, speed, totalRaces);

        auto base = std::static_pointer_cast<SrvMsg>(
                std::make_shared<SrvCurrentInfo>(std::move(ci)));
        registry->sendTo(id, base);
    }
}

void GameLoop::processLobbyCmds() {
    std::list<Cmd> to_process = emptyQueue();
    for (Cmd& cmd: to_process) {
        if (!isConnected(cmd.client_id)) {
            continue;
        }

        switch (cmd.msg->type()) {
            case (Opcode::START_GAME): {
                startRequested = true;
                break;
            }
            case (Opcode::UPGRADE_REQUEST): {
                auto it = cars.find(cmd.client_id);
                if (it == cars.end()) return;
                Car& car = it->second;
                bool success;
                Upgrade up;
                if (car.hasUpgrade()) {
                    up = NONE;
                    success = false;
                } else {

                    RequestUpgrade& ur = dynamic_cast<RequestUpgrade&>(*cmd.msg);
                    const UpgradeDef& def = findUpgradeDef(ur.getUpgrade());
                    car.applyUpgrade(def);
                    up = ur.getUpgrade();
                    success = true;
                }
                auto base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<SendUpgrade>(up, success));
                registry->sendTo(cmd.client_id, base);
                break;
            }
            case (Opcode::INIT_PLAYER): {
                bool ok = playerManager.initPlayer(cmd);
                if (ok) {
                    totalCars = static_cast<uint8_t>(cars.size());
                }
                break;
            }
            default: {
                std::cout << "cmd desconocido: " << cmd.msg->type() << "\n";
            }
        }

    }
}

void GameLoop::processCmds() {
    std::list<Cmd> to_process = emptyQueue();
    for (Cmd& cmd: to_process) {
        if (!isConnected(cmd.client_id)) {
            continue;
        }

        switch (cmd.msg->type()) {
            case (Opcode::Movement): {
                playerManager.handleMovement(cmd, TIME_STEP);
                break;
            }
            default: {
                std::cout << "cmd desconocido: " << cmd.msg->type() << "\n";
            }
        }

    }
}

void GameLoop::processWorldEvents() {
    std::unordered_set<ID> alreadyHitBuildingThisFrame;
    std::unordered_set<uint64_t> alreadyHitCarPairThisFrame;

    while (!worldEvents.empty()) {
        WorldEvent ev = worldEvents.front();
        worldEvents.pop();

        switch (ev.type) {
            case WorldEventType::CarHitCheckpoint: {
                eventHandlers.CarHitCheckpointHandler(ev);
                break;
            }
            case WorldEventType::CarHitBuilding: {
                eventHandlers.CarHitBuildingHandler(ev, alreadyHitBuildingThisFrame);
                break;
            }
            case WorldEventType::CarHitCar: {
                eventHandlers.CarHitCarHandler(ev, alreadyHitCarPairThisFrame);
                break;
            }
            default:
                break;
        }
    }
}

//para testear spawn
void GameLoop::simulatePlayerSpawns(int numPlayers) {
    if (spawnPoints.empty()) {
        std::cerr << "ERROR: No hay puntos de spawn cargados para la simulación.\n";
        return;
    }

    for (ID playerId = 2; playerId <= numPlayers; ++playerId) {
        auto base = std::static_pointer_cast<CliMsg>(
        std::make_shared<InitPlayer>("hola", CarType::CAR_GREEN));

        Cmd cmd;
        cmd.client_id = playerId;
        cmd.msg = std::move(base);

        this->queue->push(cmd);
    }
}

// la voy implementando aunque la logica del msj todavia no esta hecha
void GameLoop::disconnectHandler(ID id) {
    auto it = cars.find(id);
    if (it == cars.end()) return;
    if (!it->second.isFinished()) {
        if (totalCars > 0) {
            totalCars--;
        }
        if (totalCars > 0 && finishedCarsCount >= totalCars) {
            raceEnded = true;
        }
        if (totalCars == 0) {
            raceEnded = true;
        }
    }
    playerManager.disconnectPlayer(id);
}

std::list<Cmd> GameLoop::emptyQueue() {
    std::list<Cmd> cmd_list;
    Cmd cmd_aux;

    while (queue->try_pop(cmd_aux)) {
        cmd_list.push_back(std::move(cmd_aux));
    }
    return cmd_list;
}

void GameLoop::stop() {
    Thread::stop();
    try {
        queue->close();
    } catch (...) {}
}

GameLoop::~GameLoop() {}