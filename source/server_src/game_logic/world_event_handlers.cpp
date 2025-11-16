#include "world_event_handlers.h"
#include <box2d/box2d.h>
#include <cmath>
#include <iostream>
#include "../../common_src/srv_msg/srv_car_hit_msg.h"
#include "../../common_src/srv_msg/srv_checkpoint_hit_msg.h"

WorldEventHandlers::WorldEventHandlers(std::unordered_map<ID, Car>& cars,
                                       std::unordered_map<ID, Checkpoint>& checkpoints,
                                       ClientsRegistry& registry,
                                       float& raceTimeSeconds,
                                       int& finishedCarsCount,
                                       int& totalCars,
                                       bool& raceEnded)
        : cars(cars)
        , checkpoints(checkpoints)
        , registry(registry)
        , raceTimeSeconds(raceTimeSeconds)
        , finishedCarsCount(finishedCarsCount)
        , totalCars(totalCars)
        , raceEnded(raceEnded) {}


void WorldEventHandlers::CarHitCheckpointHandler(WorldEvent ev){
    auto itCar = cars.find(ev.carId);
    if (itCar == cars.end()) return;
    auto& car = itCar->second;

    auto actualCheckpoint = car.getActualCheckpoint();
    if (actualCheckpoint  + 1 != ev.checkpointId) return;

    car.setCheckpoint(ev.checkpointId);

    auto itCheck = checkpoints.find(ev.checkpointId);
    if (itCheck == checkpoints.end()) return;
    auto& cp = itCheck->second;

    if (cp.getKind() == CheckpointKind::Finish and !car.isFinished()) {
        car.markFinished(raceTimeSeconds);
        finishedCarsCount++;
        if (finishedCarsCount == totalCars) {
            this->raceEnded = true;
        }
    }
    auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SrvCheckpointHitMsg>(ev.carId, ev.checkpointId));
    registry.sendTo(ev.carId, msg);

}

void WorldEventHandlers::CarHitBuildingHandler(WorldEvent ev,
                                     std::unordered_set<ID>& alreadyHitBuildingThisFrame){
    if (alreadyHitBuildingThisFrame.count(ev.carId)) return;
    alreadyHitBuildingThisFrame.insert(ev.carId);

    auto it = cars.find(ev.carId);
    if (it == cars.end()) return;
    Car& car = it->second;
    b2BodyId body = car.getBody();

    b2Vec2 vel = b2Body_GetLinearVelocity(body); //vel auto

    // velocidad en la dirección del choque
    // producto interno
    // n unitario
    float impactSpeed = std::fabs(vel.x * ev.nx + vel.y * ev.ny);

    // si es muy lento pase pase
    //const float MIN_IMPACT = 1.5f;
    //if (impactSpeed < MIN_IMPACT) {
    //  break;
    //}

    // ver si fue frontal
    b2Rot rot = b2Body_GetRotation(body);
    b2Vec2 fwd = b2RotateVector(rot, {0.f, 1.f});
    // fwd es unitario
    float frontal = fwd.x * ev.nx + fwd.y * ev.ny;
    //fwd*n = ||fwd||*||n||*cos(ø)
    //como ambos son unitarios
    //fwd*n = cos(ø)


    // daño base
    float damage = impactSpeed * 0.5f;
    if (frontal > 0.7f) { // casi de frente
        damage *= 2.0f;
    }

    car.applyDamage(damage);
    if (car.isCarDestroy()) {
        //le aviso a la interfaz
    }

    // frenar un poco empujando contra la normal
    b2Vec2 newVel = {
            vel.x - ev.nx * impactSpeed * 0.5f,
            vel.y - ev.ny * impactSpeed * 0.5f
    };
    b2Body_SetLinearVelocity(body, newVel);

    auto baseCarA = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SrvCarHitMsg>(car.getClientId(), car.getHealth()));
    registry.sendTo(car.getClientId(), baseCarA);
}
void WorldEventHandlers::CarHitCarHandler(WorldEvent ev,
                                std::unordered_set<uint64_t>& alreadyHitCarPairThisFrame){
    // normalizamos la pareja (a,b) para que a < b y así no duplicamos
    int idA = ev.carId;
    int idB = ev.otherCarId;
    if (idA > idB) std::swap(idA, idB);

    uint64_t key = (static_cast<uint64_t>(idA) << 32) |
               static_cast<uint32_t>(idB);

    if (alreadyHitCarPairThisFrame.count(key)) return;
    alreadyHitCarPairThisFrame.insert(key);

    auto itA = cars.find(ev.carId);
    auto itB = cars.find(ev.otherCarId);
    if (itA == cars.end() || itB == cars.end()) return;

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

    const float MIN_IMPACT = 1.0f;
    if (aImpact < MIN_IMPACT && bImpact < MIN_IMPACT) return;

    // daño cruzado: cada uno sufre por la velocidad del otro
    float damageA = bImpact * 0.4f;
    float damageB = aImpact * 0.4f;


    carA.applyDamage(damageA);
    carB.applyDamage(damageB);

    // frenar un poco
    b2Vec2 newVA = {
            vA.x - ev.nx * aAlongN * 0.4f,
            vA.y - ev.ny * aAlongN * 0.4f
    };
    b2Vec2 newVB = {
            vB.x + ev.nx * bAlongN * 0.4f,
            vB.y + ev.ny * bAlongN * 0.4f
    };

    b2Body_SetLinearVelocity(bodyA, newVA);
    b2Body_SetLinearVelocity(bodyB, newVB);


    auto baseCarA = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SrvCarHitMsg>(carA.getClientId(), carA.getHealth()));
    registry.sendTo(carA.getClientId(), baseCarA);

    auto baseCarB = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SrvCarHitMsg>(carB.getClientId(), carB.getHealth()));
    registry.sendTo(carB.getClientId(), baseCarB);
    std::cout << "carHitCar mandanod mensaje al Server Sender\n";
}