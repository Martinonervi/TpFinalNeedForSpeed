#ifndef TPFINALNEEDFORSPEED_RACE_CONTROLLER_H
#define TPFINALNEEDFORSPEED_RACE_CONTROLLER_H

#include <memory>
#include <list>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <functional>

#include "../server_types.h"
#include "../server_client_registry.h"
#include "../world/world_manager.h"
#include "../world/entities/car.h"
#include "../world/entities/checkpoint.h"
#include "config/config_parser.h"
#include "player_manager.h"
#include "world_event_handlers.h"
#include "../../common_src/constant_rate_loop.h"
using Clock = std::chrono::steady_clock;

struct LiveRankEntry {
    ID    id;
    bool  finished;
    ID    actualCheckpoint;
    float distToNext;   // distancia al próximo checkpoint
    float finishTime;   // tiempo de carrera (si ya termino)
};


class RaceController {
public:
    RaceController(std::shared_ptr<gameLoopQueue> queue,
                   ClientsRegistry& registry,
                   WorldManager& worldManager,
                   std::queue<WorldEvent>& worldEvents,
                   std::unordered_map<ID, Car>& playerCars,
                   std::unordered_map<ID, Checkpoint>& checkpoints,
                   PlayerManager& playerManager,
                   WorldEventHandlers& eventHandlers,
                   const Config& config,
                   float& raceTimeSeconds,
                   bool& raceEnded,
                   uint8_t& totalCars,
                   uint8_t& finishedCarsCount,
                   std::vector<RaceResult>& lastRaceResults,
                   std::unordered_map<ID, Car>& npcCars);

    void runRace(uint8_t raceIndex,
                 uint8_t totalRaces,
                 const std::function<bool()>& shouldKeepRunning);

private:
    // helpers del loop de carrera
    std::list<Cmd> emptyQueue();
    void broadcastNpcCars();
    void checkPlayersStatus();
    void processCmds();
    void processWorldEvents();
    void updateRaceClockAndCheckEnd(const Clock::time_point& raceStartTime);
    void sendCurrentInfo(uint8_t raceIndex, uint8_t totalRaces);
    void finalizeDNFs();
    void sendRaceFinish();

    void disconnectHandler(ID id);
    void forcePlayerWin(ID id);
    void forcePlayerLose(ID id);
    std::vector<LiveRankEntry> buildLiveRanking();
    bool liveRankLess(const LiveRankEntry& a, const LiveRankEntry& b);

    // referencias a cosas del servidor
    std::shared_ptr<gameLoopQueue> queue;
    ClientsRegistry&               registry;
    WorldManager&                  worldManager;
    std::queue<WorldEvent>&        worldEvents;
    std::unordered_map<ID, Car>&   playerCars;
    std::unordered_map<ID, Checkpoint>& checkpoints;
    PlayerManager&                 playerManager;
    WorldEventHandlers&            eventHandlers;
    std::unordered_map<ID, Car>&        npcCars;
    const Config&                  config;

    // estado compartido con GameLoop / WorldEventHandlers
    float&    raceTimeSeconds;
    bool&     raceEnded;
    uint8_t&  totalCars;
    uint8_t&  finishedCarsCount;
    std::vector<RaceResult>& lastRaceResults;
};

#endif
