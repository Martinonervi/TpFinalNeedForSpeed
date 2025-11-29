#ifndef TPFINALNEEDFORSPEED_WORLD_EVENT_HANDLERS_H
#define TPFINALNEEDFORSPEED_WORLD_EVENT_HANDLERS_H

#include <unordered_set>
#include "../world/world_manager.h"
#include "../server_client_registry.h"
#include "../world/entities/car.h"
#include "../world/entities/checkpoint.h"

struct RaceResult {
    ID playerId;
    float raceTime;       // tiempo en esta carrera (en segundos)
    int racePosition;     // 1,2,3,... según orden de llegada
    bool finished;        // por si alguno no terminó
};

class WorldEventHandlers {
public:
    WorldEventHandlers(std::unordered_map<ID, Car>& playerCars,
                       std::unordered_map<ID, Checkpoint>& checkpoints,
                       ClientsRegistry& registry,
                       float& raceTimeSeconds,
                       uint8_t& finishedCarsCount,
                       uint8_t& totalCars,
                       bool& raceEnded,
                       std::vector<ID>& raceRanking,
                       std::vector<RaceResult>& lastRaceResults);


    void CarHitCheckpointHandler(WorldEvent ev);
    void CarHitBuildingHandler(WorldEvent ev, std::unordered_set<ID>& alreadyHitBuildingThisFrame);
    void CarHitCarHandler(WorldEvent ev, std::unordered_set<uint64_t>& alreadyHitCarPairThisFrame);

    void onPlayerFinishedRace(ID playerId, float timeSec);
    void CarFinishRace(Car& car);

private:
    std::unordered_map<ID, Car>& playerCars;
    std::unordered_map<ID, Checkpoint>& checkpoints;
    ClientsRegistry& registry;

    float& raceTimeSeconds;
    uint8_t& finishedCarsCount;
    uint8_t& totalCars;
    bool& raceEnded;
    std::vector<ID>& raceRanking;
    std::vector<RaceResult>& lastRaceResults;
};



#endif
