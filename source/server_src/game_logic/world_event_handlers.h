#ifndef TPFINALNEEDFORSPEED_WORLD_EVENT_HANDLERS_H
#define TPFINALNEEDFORSPEED_WORLD_EVENT_HANDLERS_H

#include <unordered_set>
#include "../world/world_manager.h"
#include "../server_client_registry.h"
#include "../world/entities/car.h"
#include "../world/entities/checkpoint.h"
#include "config/config_parser.h"

struct RaceResult {
    ID playerId;
    float raceTime;
    int racePosition;
    bool finished;
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
                       std::vector<RaceResult>& lastRaceResults,
                       const CollisionsConfig& collisionsConfig);

    // checkpoints
    void CarHitCheckpointHandler(WorldEvent ev);

    // colisiones
    void CarHitBuildingHandler(WorldEvent ev, std::unordered_set<ID>& alreadyHitBuildingThisFrame);
    void CarHitCarHandler(WorldEvent ev, std::unordered_set<uint64_t>& alreadyHitCarPairThisFrame);

    // fin carrera / auto destruido
    void CarFinishRace(Car& car);
    void setKillCar(Car& car);
    void freezeAndDisableCarBody(Car& car);

private:
    void onPlayerFinishedRace(ID playerId, float timeSec, Car& car);
    void broadcastCarHealth(const Car& car);
    float velocityAlongNormal(const Car& car,float nx, float ny) const;//proyección sobre la normal
    float frontalAlignment(const Car& car, float nx, float ny) const;
    void applyVelocityDamp(b2BodyId body, float nx, float ny,
        float alongN, float damp, float directionSign);

    // referencias al estado del juego
    std::unordered_map<ID, Car>& playerCars;
    std::unordered_map<ID, Checkpoint>& checkpoints;
    ClientsRegistry& registry;

    float& raceTimeSeconds;
    uint8_t& finishedCarsCount;
    uint8_t& totalCars;
    bool& raceEnded;
    std::vector<ID>& raceRanking;
    std::vector<RaceResult>& lastRaceResults;

    const CollisionsConfig& collisionsConfig;
};



#endif
