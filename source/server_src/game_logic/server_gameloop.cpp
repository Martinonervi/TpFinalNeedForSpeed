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
#include "../../common_src/srv_msg/srv_starting_game.h"

#define FILE_YAML_PATH "../server_src/world/map.yaml"
using Clock = std::chrono::steady_clock;


GameLoop::GameLoop(std::shared_ptr<gameLoopQueue> queue,
                   std::shared_ptr<ClientsRegistry> registry, const Config& config)
    : config(config)
    , maxPlayers(config.lobby.maxPlayers)
    , worldEvents()
    , worldManager(worldEvents)
    , queue(std::move(queue))
    , registry(std::move(registry))
    , eventHandlers(playerCars, checkpoints, *this->registry,
                    raceTimeSeconds, finishedCarsCount, totalCars,
                    raceEnded, raceRanking, lastRaceResults, config.collisions)
    , upgrades(config.upgrades)
    , playerManager(worldManager, *this->registry,playerCars,
                    spawnPoints,raceStarted, checkpoints, upgrades, config)
    , lobbyController(this->queue,*this->registry, playerManager,
                      playerCars, upgrades, config, startRequested,
                      raceStarted, totalCars)
    , raceController(this->queue, *this->registry, worldManager,
                     worldEvents,playerCars,checkpoints,
                     playerManager, eventHandlers, config,
                     raceTimeSeconds, raceEnded, totalCars,
                     finishedCarsCount, lastRaceResults, npcCars)
{
    loadMapFromYaml(FILE_YAML_PATH);
}

CarType carTypeFromString(const std::string& s) {
    if (s == "green")  return CarType::CAR_GREEN;
    if (s == "red")    return CarType::CAR_RED;
    if (s == "porsche") return CarType::CAR_PORSCHE;
    if (s == "lightBlue") return CarType::CAR_LIGHT_BLUE;
    if (s == "jeep") return CarType::CAR_JEEP;
    if (s == "pickup") return CarType::CAR_PICKUP;
    if (s == "limo") return CarType::CAR_LIMO;
    return CarType::CAR_GREEN;
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
    createNpcCars();
}

// loop principal de distinas carreras
void GameLoop::run() {
    setupRoute();

    this->totalRaces = mapData.routes.size(); // seria el raceToPlay,
                                              // se lo paso para ver si le llega bien a fran
    const int racesToPlay = mapData.routes.size();

    while (raceIndex < racesToPlay && should_keep_running()) {
        lobbyController.runLobbyLoop(raceIndex, recommendedPath,
            [this]() { return this->should_keep_running(); }
            );

        raceController.runRace(
            raceIndex, totalRaces,
            [this]() { return this->should_keep_running(); }
            );

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

void GameLoop::createNpcCars() {
    const ID NPC_BASE_ID = 100;

    npcCars.clear();

    for (size_t i = 0; i < mapData.npcParked.size(); ++i) {
        const auto& cfg = mapData.npcParked[i];
        ID npcId = NPC_BASE_ID + static_cast<ID>(i);

        b2Vec2 pos{ cfg.x, cfg.y };
        float angleRad = cfg.angle;
        CarType type = carTypeFromString(cfg.carType);

        auto [it, ok] = npcCars.emplace(
            npcId,
            Car(worldManager, npcId, pos, angleRad, type, config.carHandling)
        );

        if (ok) {
            b2BodyId body = it->second.getBody();
            std::cout << "[NPC] creado id=" << npcId
                      << " body=" << body.index1
                      << " pos=(" << pos.x << "," << pos.y << ")\n";
        }
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
}

void GameLoop::resetRaceState() {
    // saca checkpoints viejos
    worldManager.resetCheckpoints(checkpoints);
    // carga nueva ruta
    setupRoute(); // checkpoints, spawnPoints, recommendedPath

    // reseteo posicion y estado de los autos
    uint8_t i = 0;
    for (auto& [id, car] : playerCars) {
        const auto& sp = spawnPoints[i % spawnPoints.size()];
        car.resetForNewRace(sp.x, sp.y, sp.angle);
        i++;
    }

    // reseteo variables de estado de la carrera
    raceTimeSeconds    = 0.f;
    finishedCarsCount  = 0;
    totalCars          = static_cast<uint8_t>(playerCars.size());
    raceEnded          = false;
    raceStarted        = false;
    startRequested     = false;

    raceRanking.clear();
    lastRaceResults.clear();

    // vacio queue de eventos del mundo
    while (!worldEvents.empty()) {
        worldEvents.pop();
    }

    // vacio los comandos de la carrera anterior
    Cmd cmd_aux;
    try {
        while (queue->try_pop(cmd_aux)) { }
    } catch (const ClosedQueue&) {
        // la queue ya fue cerrada por stop
    }

}

bool GameLoop::isRaceStarted() const {
    return this->raceStarted;
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




void GameLoop::stop() {
    Thread::stop();
    try {
        queue->close();
    } catch (...) {}
}

GameLoop::~GameLoop() {}



