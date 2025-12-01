#include "world_event_handlers.h"
#include <box2d/box2d.h>
#include <cmath>
#include <iostream>
#include "../../common_src/srv_msg/srv_car_hit_msg.h"
#include "../../common_src/srv_msg/srv_checkpoint_hit_msg.h"

WorldEventHandlers::WorldEventHandlers(std::unordered_map<ID, Car>& playerCars,
                                       std::unordered_map<ID, Checkpoint>& checkpoints,
                                       ClientsRegistry& registry,
                                       float& raceTimeSeconds,
                                       uint8_t& finishedCarsCount,
                                       uint8_t& totalCars,
                                       bool& raceEnded,  std::vector<ID>& raceRanking,
                                       std::vector<RaceResult>& lastRaceResults,
                                       const CollisionsConfig& collisionsConfig)
        : playerCars(playerCars)
        , checkpoints(checkpoints)
        , registry(registry)
        , raceTimeSeconds(raceTimeSeconds)
        , finishedCarsCount(finishedCarsCount)
        , totalCars(totalCars)
        , raceEnded(raceEnded)
        ,raceRanking(raceRanking)
        ,lastRaceResults(lastRaceResults)
        ,collisionsConfig(collisionsConfig) {}


void WorldEventHandlers::CarHitCheckpointHandler(WorldEvent ev){
    auto itCar = playerCars.find(ev.carId);
    if (itCar == playerCars.end()) return;
    auto& car = itCar->second;

    auto actualCheckpoint = car.getActualCheckpoint();
    if (actualCheckpoint  + 1 != ev.checkpointId) return;

    car.setCheckpoint(ev.checkpointId);

    auto itCheck = checkpoints.find(ev.checkpointId);
    if (itCheck == checkpoints.end()) return;
    auto& cp = itCheck->second;

    if (cp.getKind() == CheckpointKind::Finish and !car.isFinished()) {
        CarFinishRace(car);
    }
    auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SrvCheckpointHitMsg>(ev.carId, ev.checkpointId));
    registry.sendTo(ev.carId, msg);
}


void WorldEventHandlers::CarHitBuildingHandler(WorldEvent ev,
    std::unordered_set<ID>& alreadyHitBuildingThisFrame) {
    if (alreadyHitBuildingThisFrame.count(ev.carId)) return;
    alreadyHitBuildingThisFrame.insert(ev.carId);

    auto it = playerCars.find(ev.carId);
    if (it == playerCars.end()) return;
    Car& car = it->second;
    b2BodyId body = car.getBody();

    const auto& cfg = collisionsConfig.building;

    float impactSpeed = std::fabs(velocityAlongNormal(car, ev.nx, ev.ny));
    if (impactSpeed < cfg.minImpactSpeed) return; //ignoramos golpe suave

    float frontal = frontalAlignment(car, ev.nx, ev.ny);
    float damage = impactSpeed * cfg.baseDamageFactor;
    if (frontal > cfg.minFrontalAlignment) { // casi de frente
        damage *= cfg.frontalMultiplier;
    }

    car.applyDamage(damage);

    if (car.isCarDestroy()) {
        setKillCar(car);
    } else {
        applyVelocityDamp(body, ev.nx, ev.ny, impactSpeed, cfg.velocityDampFactor, 1.0f);
    }
    broadcastCarHealth(car);
}

void WorldEventHandlers::CarHitCarHandler(WorldEvent ev,
    std::unordered_set<uint64_t>& alreadyHitCarPairThisFrame) {
    int idA = ev.carId;
    int idB = ev.otherCarId;
    if (idA > idB) std::swap(idA, idB);
    uint64_t key = (static_cast<uint64_t>(idA) << 32) | static_cast<uint32_t>(idB);
    if (alreadyHitCarPairThisFrame.count(key)) return;
    alreadyHitCarPairThisFrame.insert(key);

    auto itA = playerCars.find(ev.carId);
    auto itB = playerCars.find(ev.otherCarId);
    if (itA == playerCars.end() || itB == playerCars.end()) return;
    Car& carA = itA->second;
    Car& carB = itB->second;
    b2BodyId bodyA = carA.getBody();
    b2BodyId bodyB = carB.getBody();
    const auto& cfg = collisionsConfig.car;

    float aAlongN = velocityAlongNormal(carA, ev.nx, ev.ny);
    float bAlongN = velocityAlongNormal(carB, ev.nx, ev.ny);
    float aImpact = std::fabs(aAlongN);
    float bImpact = std::fabs(bAlongN);
    if (aImpact < cfg.minImpactSpeed && bImpact < cfg.minImpactSpeed) return;

    float frontal = frontalAlignment(carA, ev.nx, ev.ny);
    float damageA = bImpact * cfg.baseDamageFactor * carB.getDamage();
    float damageB = aImpact * cfg.baseDamageFactor * carA.getDamage();
    if (frontal > cfg.minFrontalAlignment) {
        damageA *= cfg.frontalMultiplier;
        damageB *= cfg.frontalMultiplier;
    }

    carA.applyDamage(damageA);
    carB.applyDamage(damageB);
    applyVelocityDamp(bodyA, ev.nx, ev.ny, aAlongN, cfg.velocityDampFactor,  1.0f);
    applyVelocityDamp(bodyB, ev.nx, ev.ny, bAlongN, cfg.velocityDampFactor, -1.0f);

    if (carA.isCarDestroy()) setKillCar(carA);
    if (carB.isCarDestroy()) setKillCar(carB);
    broadcastCarHealth(carA);
    broadcastCarHealth(carB);
}



void WorldEventHandlers::onPlayerFinishedRace(ID playerId, float timeSec) {
    lastRaceResults.push_back(RaceResult{
            playerId,
            timeSec,
            finishedCarsCount,
            true
    });
}

void WorldEventHandlers::CarFinishRace(Car& car) {
    finishedCarsCount++;
    car.markFinished(raceTimeSeconds, finishedCarsCount);
    onPlayerFinishedRace(car.getClientId(), raceTimeSeconds);
    raceRanking.push_back(car.getClientId());
    freezeAndDisableCarBody(car);
    if (finishedCarsCount == totalCars) {
        raceEnded = true;
    }
}

void WorldEventHandlers::setKillCar(Car& car) {
    freezeAndDisableCarBody(car);
    car.kill();
    totalCars -= 1;
}

void WorldEventHandlers::freezeAndDisableCarBody(Car& car) {
    b2BodyId body = car.getBody();
    b2Vec2 zero{0.f, 0.f};
    b2Body_SetLinearVelocity(body, zero);
    b2Body_SetAngularVelocity(body, 0.f);
    b2Body_Disable(body);
}

void WorldEventHandlers::broadcastCarHealth(const Car& car) {
    auto msg = std::static_pointer_cast<SrvMsg>(
        std::make_shared<SrvCarHitMsg>(
            car.getClientId(),
            car.getHealth(),
            car.getTotalHealth()));
    registry.broadcast(msg);
}


float WorldEventHandlers::velocityAlongNormal(const Car& car,
                                              float nx, float ny) const {
    b2BodyId body = car.getBody();
    b2Vec2 v = b2Body_GetLinearVelocity(body);
    return v.x * nx + v.y * ny;
}

float WorldEventHandlers::frontalAlignment(const Car& car,
                                           float nx, float ny) const {
    b2BodyId body = car.getBody();
    b2Rot rot = b2Body_GetRotation(body);
    b2Vec2 fwd = b2RotateVector(rot, {0.f, 1.f});  // "para adelante" del auto
    return fwd.x * nx + fwd.y * ny; // cos(ángulo) entre fwd y n
    //fwd*n = ||fwd||*||n||*cos(ø) (ambos unitarios) => cos(ø)
}

void WorldEventHandlers::applyVelocityDamp(
        b2BodyId body,
        float nx, float ny,
        float alongN,
        float damp,
        float directionSign) {
    b2Vec2 v = b2Body_GetLinearVelocity(body);
    b2Vec2 newV = {
        v.x - directionSign * nx * alongN * damp,
        v.y - directionSign * ny * alongN * damp
    };
    b2Body_SetLinearVelocity(body, newV);
}

