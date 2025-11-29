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
                                       std::vector<RaceResult>& lastRaceResults, Config& config)
        : playerCars(playerCars)
        , checkpoints(checkpoints)
        , registry(registry)
        , raceTimeSeconds(raceTimeSeconds)
        , finishedCarsCount(finishedCarsCount)
        , totalCars(totalCars)
        , raceEnded(raceEnded)
        ,raceRanking(raceRanking)
        ,lastRaceResults(lastRaceResults)
        ,config(config) {}


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


void WorldEventHandlers::CarHitBuildingHandler(
        WorldEvent ev,
        std::unordered_set<ID>& alreadyHitBuildingThisFrame) {

    if (alreadyHitBuildingThisFrame.count(ev.carId)) return;
    alreadyHitBuildingThisFrame.insert(ev.carId);

    auto it = playerCars.find(ev.carId);
    if (it == playerCars.end()) return;
    Car& car = it->second;
    b2BodyId body = car.getBody();

    b2Vec2 vel = b2Body_GetLinearVelocity(body);

    const auto& cfg = config.collisions.building;

    const float MIN_IMPACT            = cfg.minImpactSpeed;
    const float DAMAGE_FACTOR         = cfg.baseDamageFactor;
    const float MIN_FRONTAL_ALIGNMENT = cfg.minFrontalAlignment;
    const float FRONTAL_MULT          = cfg.frontalMultiplier;
    const float VELOCITY_DAMP         = cfg.velocityDampFactor;

    // velocidad de impacto en la dirección de la normal
    // velocidad en la dirección del choque
    // producto interno
    // n unitario
    float impactSpeed = std::fabs(vel.x * ev.nx + vel.y * ev.ny);

    // si el golpe es muy suave, lo ignoramos
    if (impactSpeed < MIN_IMPACT) return;

    // ver si el choque fue frontal
    b2Rot rot = b2Body_GetRotation(body);
    b2Vec2 fwd = b2RotateVector(rot, {0.f, 1.f});  // "para adelante" del auto
    float frontal = fwd.x * ev.nx + fwd.y * ev.ny; // cos(ángulo) entre fwd y n
    // fwd es unitario
    //fwd*n = ||fwd||*||n||*cos(ø)
    //como ambos son unitarios
    //fwd*n = cos(ø)


    float damage = impactSpeed * DAMAGE_FACTOR;
    if (frontal > MIN_FRONTAL_ALIGNMENT) { // casi de frente
        damage *= FRONTAL_MULT;
    }

    car.applyDamage(damage);

    if (car.isCarDestroy()) {
        setKillCar(car);
    } else {
        b2Vec2 newVel = {
                vel.x - ev.nx * impactSpeed * VELOCITY_DAMP,
                vel.y - ev.ny * impactSpeed * VELOCITY_DAMP
        };
        b2Body_SetLinearVelocity(body, newVel);
    }

    auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SrvCarHitMsg>(car.getClientId(), car.getHealth(),
                car.getTotalHealth()));
    registry.broadcast(msg);
}
void WorldEventHandlers::setKillCar(Car& car) {
    b2BodyId body = car.getBody();
    b2Vec2 zero = {0.f, 0.f};
    b2Body_SetLinearVelocity(body, zero);
    b2Body_SetAngularVelocity(body, 0.f);
    car.kill();
    totalCars -= 1;
}

void WorldEventHandlers::CarHitCarHandler(
    WorldEvent ev,
    std::unordered_set<uint64_t>& alreadyHitCarPairThisFrame) {
    int idA = ev.carId;
    int idB = ev.otherCarId;
    if (idA > idB) std::swap(idA, idB);

    uint64_t key = (static_cast<uint64_t>(idA) << 32) |
                   static_cast<uint32_t>(idB);

    if (alreadyHitCarPairThisFrame.count(key)) return;
    alreadyHitCarPairThisFrame.insert(key);

    auto itA = playerCars.find(ev.carId);
    auto itB = playerCars.find(ev.otherCarId);
    if (itA == playerCars.end() || itB == playerCars.end()) return;

    Car& carA = itA->second;
    Car& carB = itB->second;

    b2BodyId bodyA = carA.getBody();
    b2BodyId bodyB = carB.getBody();

    b2Vec2 vA = b2Body_GetLinearVelocity(bodyA);
    b2Vec2 vB = b2Body_GetLinearVelocity(bodyB);

    float aAlongN = vA.x * ev.nx + vA.y * ev.ny;
    float bAlongN = vB.x * ev.nx + vB.y * ev.ny;

    float aImpact = std::fabs(aAlongN);
    float bImpact = std::fabs(bAlongN);

    const auto& cfg = config.collisions.car;

    const float MIN_IMPACT             = cfg.minImpactSpeed;
    const float DAMAGE_FACTOR          = cfg.baseDamageFactor;
    const float MIN_FRONTAL_ALIGNMENT  = cfg.minFrontalAlignment;
    const float FRONTAL_MULT           = cfg.frontalMultiplier;
    const float VELOCITY_DAMP          = cfg.velocityDampFactor;


    if (aImpact < MIN_IMPACT && bImpact < MIN_IMPACT) return;

    b2Rot rotA = b2Body_GetRotation(bodyA);
    b2Vec2 fwdA = b2RotateVector(rotA, {0.f, 1.f});
    float frontal = fwdA.x * ev.nx + fwdA.y * ev.ny;

    float damageA = bImpact * DAMAGE_FACTOR * carB.getDamage();
    float damageB = aImpact * DAMAGE_FACTOR * carA.getDamage();

    // si el choque es bastante frontal según A, amplificamos daño para los dos
    if (frontal > MIN_FRONTAL_ALIGNMENT) {
        damageA *= FRONTAL_MULT;
        damageB *= FRONTAL_MULT;
    }

    carA.applyDamage(damageA);
    carB.applyDamage(damageB);

    // frenar / rebotar un poco en la dirección de la normal
    b2Vec2 newVA = {
            vA.x - ev.nx * aAlongN * VELOCITY_DAMP,
            vA.y - ev.ny * aAlongN * VELOCITY_DAMP
    };
    b2Vec2 newVB = {
            vB.x + ev.nx * bAlongN * VELOCITY_DAMP,
            vB.y + ev.ny * bAlongN * VELOCITY_DAMP
    };

    b2Body_SetLinearVelocity(bodyA, newVA);
    b2Body_SetLinearVelocity(bodyB, newVB);

    if (carA.isCarDestroy()) {
        setKillCar(carA);
    }

    if (carB.isCarDestroy()) {
        setKillCar(carB);
    }

    auto baseCarA = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SrvCarHitMsg>(carA.getClientId(), carA.getHealth(),
                carA.getTotalHealth()));
    registry.broadcast(baseCarA);

    auto baseCarB = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SrvCarHitMsg>(carB.getClientId(), carB.getHealth(),
                carB.getTotalHealth()));
    registry.broadcast(baseCarB);
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
    if (finishedCarsCount == totalCars) {
        raceEnded = true;
    }
}

