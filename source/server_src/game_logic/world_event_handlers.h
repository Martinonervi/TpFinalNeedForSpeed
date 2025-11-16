#ifndef TPFINALNEEDFORSPEED_WORLD_EVENT_HANDLERS_H
#define TPFINALNEEDFORSPEED_WORLD_EVENT_HANDLERS_H

#include <unordered_set>
#include "../world/world_manager.h"
#include "../server_client_registry.h"
#include "../world/entities/car.h"
#include "../world/entities/checkpoint.h"

class WorldEventHandlers {
public:
    WorldEventHandlers(std::unordered_map<ID, Car>& cars,
                       std::unordered_map<ID, Checkpoint>& checkpoints,
                       ClientsRegistry& registry,
                       float& raceTimeSeconds,
                       int& finishedCarsCount,
                       int& totalCars,
                       bool& raceEnded);


    void CarHitCheckpointHandler(WorldEvent ev);
    void CarHitBuildingHandler(WorldEvent ev, std::unordered_set<ID>& alreadyHitBuildingThisFrame);
    void CarHitCarHandler(WorldEvent ev, std::unordered_set<uint64_t>& alreadyHitCarPairThisFrame);

private:
    std::unordered_map<ID, Car>& cars;
    std::unordered_map<ID, Checkpoint>& checkpoints;
    ClientsRegistry& registry;

    float& raceTimeSeconds;
    int& finishedCarsCount;
    int& totalCars;
    bool& raceEnded;
};



#endif
