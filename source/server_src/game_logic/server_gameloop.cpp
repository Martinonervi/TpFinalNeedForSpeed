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
#include "../../common_src/srv_msg/srv_upgrade_logic.h"

#define FILE_YAML_PATH "../server_src/world/map.yaml"
using Clock = std::chrono::steady_clock;
const int MAX_PLAYERS = 8; //monitor, hablarlo con feli


GameLoop::GameLoop(std::shared_ptr<gameLoopQueue> queue, std::shared_ptr<ClientsRegistry> registry):
config(ConfigParser().load("../server_src/game_logic/config/config.yaml")),
worldEvents(), worldManager(worldEvents),queue(std::move(queue)),
registry(std::move(registry)), eventHandlers(playerCars, checkpoints, *this->registry,
            raceTimeSeconds, finishedCarsCount, totalCars, raceEnded, raceRanking, lastRaceResults, config),
upgrades(config.upgrades),
        playerManager(worldManager, *this->registry, playerCars, spawnPoints, raceStarted, checkpoints, config)  {
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

    this->totalRaces = mapData.routes.size(); // seria el raceToPlay,
                                              // se lo paso para ver si le llega bien a fran
    const int racesToPlay = mapData.routes.size();

    while (raceIndex < racesToPlay && should_keep_running()) {

        waitingForPlayers();
        runSingleRace();

        updateGlobalStatsFromLastRace(); // acumula tiempos globales


        raceIndex += 1;
        if (raceIndex < racesToPlay) { //no termino la ultima
            resetRaceState();
        }
    }
    if (should_keep_running()) {
        computeGlobalRanking();
        playerManager.sendPlayerStats(globalStats);
    }
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
    recommendedPath.clear();

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
    recommendedPath = routeCfg.recommendedPath;

    /*std::cout << "[GameLoop] setupRoute: usando ruta '"
              << routeCfg.nameRoute
              << "' (index=" << routeIndex
              << "), checkpoints=" << checkpoints.size()
              << ", spawns=" << spawnPoints.size()
              << "\n";
              */
}



void GameLoop::waitingForPlayers() {
    ConstantRateLoop loop(config.loops.lobbyHz);

    startRequested = false;
    raceStarted    = false;

    auto start = Clock::now();
    const auto deadline = start + std::chrono::duration<double>(config.lobby.betweenRacesSec);
    while (should_keep_running()) {
        processLobbyCmds();

        if (startRequested) break;
        if (raceIndex != 0 and Clock::now() >= deadline) break;

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

        // mando el recommendedPath
        auto rp = std::static_pointer_cast<SrvMsg>(
                std::make_shared<RecommendedPath>(recommendedPath));
        registry->broadcast(rp);

        auto ul = std::static_pointer_cast<SrvMsg>(
                std::make_shared<UpgradeLogic>(upgrades));
        registry->broadcast(ul);

        loop.sleep_until_next_frame();
    }
    this->raceStarted = true;
}

void GameLoop::resetRaceState() {
    worldManager.resetCheckpoints(checkpoints);
    setupRoute();

    uint8_t i = 0;
    for (auto& [id, car] : playerCars) {
        const auto& sp = spawnPoints[i % spawnPoints.size()];
        car.resetForNewRace(sp.x, sp.y, sp.angle);
        i++;
    }

    // reseteo variables
    raceTimeSeconds    = 0.f;
    finishedCarsCount  = 0;
    totalCars          = static_cast<int>(playerCars.size());
    raceEnded          = false;
    raceRanking.clear();
    lastRaceResults.clear();

    // vaciar queues
    while (!worldEvents.empty()) {
        worldEvents.pop();
    }
    if (raceIndex > 0) {
        Cmd cmd_aux;
        try {
            while (queue->try_pop(cmd_aux)) { }
        } catch (const ClosedQueue&) {
            // la queue ya fue cerrada por stop
        }
    }
}



void GameLoop::runSingleRace() {
    raceStartTime = Clock::now();
    try {
        ConstantRateLoop loop(config.loops.raceHz);

        while (should_keep_running()) {
            checkPlayersStatus();
            processCmds();
            worldManager.step(config.physics.timeStep, config.physics.subStepCount);

            if (!raceEnded) {
                auto now = Clock::now();
                std::chrono::duration<float> elapsed = now - raceStartTime;
                raceTimeSeconds = elapsed.count();

                if (raceTimeSeconds >= config.lobby.maxRaceTimeSec) {
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
    finalizeDNFs();
}


void GameLoop::checkPlayersStatus() {
    std::vector<ID> ids;
    for (auto& car: playerCars) {
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
    for (auto& [id, car] : playerCars) {
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
        uint8_t totalCheckpoints = checkpoints.size();
        SrvCurrentInfo ci(cp.getId(), cp.getX(), cp.getY(), angle, len,
                           config.lobby.maxRaceTimeSec - raceTimeSeconds, raceIndex+1, speed,
                           totalRaces, totalCheckpoints);

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
                //comentar tiempo break
                break;
            }
            case (Opcode::UPGRADE_REQUEST): {
                auto it = playerCars.find(cmd.client_id);
                if (it == playerCars.end()) continue;
                Car& car = it->second;
                bool success;
                Upgrade up;

                auto& ur = dynamic_cast<RequestUpgrade&>(*cmd.msg);
                const UpgradeDef& def = findUpgradeDef(ur.getUpgrade());

                if (car.hasMaxUpgrade() || !car.applyUpgrade(def)) {
                    //std::cout << "[GameLoop] upgrade rechazado para id=" << cmd.client_id << "\n";
                    up = NONE;
                    success = false;
                } else {
                    //std::cout << "[GameLoop] upgrade ACEPTADO para id=" << cmd.client_id
              //<< " upgrade=" << (int)ur.getUpgrade() << "\n";
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
                    totalCars = static_cast<uint8_t>(playerCars.size());
                }
                break;
            }
            default: {
                continue;
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
                playerManager.handleMovement(cmd, config.physics.timeStep);
                break;
            }
            case (Opcode::INIT_PLAYER): {
                playerManager.initPlayer(cmd);
                break;
            }
            case (Opcode::REQUEST_CHEAT): {
                if (!config.cheats.enabled) break;
                const auto& cr = dynamic_cast<const CheatRequest&>(*cmd.msg);
                Cheat cheat = cr.getCheat();

                switch (cheat) {
                    case Cheat::HEALTH_CHEAT:
                    case Cheat::FREE_SPEED_CHEAT:
                    case Cheat::NEXT_CHECKPOINT_CHEAT:
                        playerManager.cheatHandler(cmd); // cheats “del auto”
                        break;

                    case Cheat::WIN_RACE_CHEAT:
                        if (!config.cheats.allowWinRaceCheat) break;
                        forcePlayerWin(cmd.client_id);
                        break;

                    case Cheat::LOST_RACE_CHEAT:
                        if (!config.cheats.allowLostRaceCheat) break;
                        //forcePlayerLose(cmd.client_id);
                        break;

                    default:
                        std::cout << "[GameLoop] cheat desconocido: " << (int)cheat << "\n";
                        break;
                }
                break;
            }

            default: {
                std::cout << "[Gameloop] comando desconocido: " << static_cast<int>(cmd.msg->type()) << "\n";
            }
        }

    }
}


void GameLoop::forcePlayerWin(ID id) {
    auto itCar = playerCars.find(id);
    if (itCar == playerCars.end()) return;

    Car& car = itCar->second;
    if (car.isFinished()) return;

    car.setCheckpoint(checkpoints.size());
    eventHandlers.CarFinishRace(car);
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
    auto it = playerCars.find(id);
    if (it == playerCars.end()) return;
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
    try {
        while (queue->try_pop(cmd_aux)) {
            cmd_list.push_back(std::move(cmd_aux));
        }
    } catch (const ClosedQueue&) {
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


void GameLoop::finalizeDNFs() {
    for (auto& [id, car] : playerCars) {
        if (!car.isFinished()) {
            lastRaceResults.push_back(RaceResult{
                    id,
                    raceTimeSeconds,
                    0,
                    false
            });
        }
    }
}


void GameLoop::updateGlobalStatsFromLastRace() {
    for (const auto& r : lastRaceResults) {
        auto& gs = globalStats[r.playerId];  // crea la entrada si no existe
        gs.totalTime += r.raceTime;
    }
}

void GameLoop::computeGlobalRanking() {
    std::vector<std::pair<ID, PlayerGlobalStats*>> ranking;
    ranking.reserve(globalStats.size());

    for (auto& [id, stats] : globalStats) {
        // le sumo la penalidad TOTAL que acumuló el auto en toda la partida
        auto it = playerCars.find(id);
        if (it != playerCars.end()) {
            stats.totalTime += it->second.getPenalty();
        }

        ranking.push_back({id, &stats});
    }

    std::sort(ranking.begin(), ranking.end(),
              [](const auto& a, const auto& b) {
                  return a.second->totalTime < b.second->totalTime;
              });

    uint8_t pos = 1;
    for (auto& [id, statsPtr] : ranking) {
        statsPtr->globalPosition = pos++;
    }
}


