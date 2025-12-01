#ifndef SERVER_GAMELOOP_H
#define SERVER_GAMELOOP_H

#include <list>
#include <map>
#include <unordered_set>
#include <queue>
#include <memory>
#include <chrono>

#include <box2d/box2d.h>

#include "../server_client_registry.h"
#include "../server_types.h"

#include "../world/world_manager.h"
#include "../world/map_parser.h"
#include "../world/entities/building.h"
#include "../world/entities/car.h"
#include "../world/entities/checkpoint.h"

#include "config/config_parser.h"
#include "world_event_handlers.h"
#include "player_manager.h"
#include "lobby_controller.h"
#include "race_controller.h"

class GameLoop : public Thread {
public:
    GameLoop(std::shared_ptr<gameLoopQueue> queue,
             std::shared_ptr<ClientsRegistry> registry);

    ~GameLoop() override;

    void stop() override;
    bool isRaceStarted() const;
    uint8_t getMaxPlayers() const { return maxPlayers; }

protected:
    void run() override;

private:
    // estructura de la partida
    void loadMapFromYaml(const std::string& path);
    void setupRoute();        // setea checkpoints / spawns / recommendedPath según raceIndex
    void resetRaceState();    // resetea autos, timers y colas entre carreras

    // otros helpers
    void disconnectHandler(ID id);
    void broadcastNpcCars();

    // stats globales de la sesión
    void updateGlobalStatsFromLastRace();  // acumula tiempos totales
    void computeGlobalRanking();           // ordena por tiempo total + penalidades

    void createNpcCars();

    Config  config;
    uint8_t maxPlayers;


    std::shared_ptr<gameLoopQueue>   queue;
    std::shared_ptr<ClientsRegistry> registry;

    // mundo físico / eventos
    std::queue<WorldEvent> worldEvents;
    WorldManager           worldManager;

    // entidades
    std::unordered_map<ID, Car>        playerCars;
    std::unordered_map<ID, Checkpoint> checkpoints;
    std::vector<std::unique_ptr<Building>> buildings;
    std::vector<SpawnPointConfig>      spawnPoints;
    std::vector<RecommendedPoint>      recommendedPath;
    std::unordered_map<ID, Car>        npcCars;

    // upgrades
    std::vector<UpgradeDef> upgrades;

    // manejadores de lógica
    WorldEventHandlers eventHandlers;
    PlayerManager      playerManager;
    LobbyController    lobbyController;
    RaceController     raceController;

    // estado de carrera actual
    float raceTimeSeconds = 0.0f;
    std::chrono::steady_clock::time_point raceStartTime{};

    bool    raceStarted       = false;
    bool    raceEnded         = false;
    bool    startRequested    = false;
    uint8_t finishedCarsCount = 0;
    uint8_t totalCars         = 0;

    // info de rutas / partidas
    uint8_t raceIndex  = 0;   // índice de ruta actual
    uint8_t totalRaces = 0;   // cantidad total de rutas del mapa
    MapData mapData;

    // ranking de la carrera actual + stats globales
    std::vector<ID>                 raceRanking;
    std::vector<RaceResult>         lastRaceResults;
    std::unordered_map<ID, PlayerGlobalStats> globalStats;

};

#endif
