#pragma once
#include <box2d/box2d.h>

#include "../../../common_src/cli_msg/move_Info.h"
#include "../../../common_src/constants.h"
#include "../../../common_src/srv_msg/player_state.h"

#include "entity.h"


class Car : public Entity {
public:
    Car(WorldManager& world,
        ID clientId,
        b2Vec2 pos,
        float angleRad,
        CarType carType);

    void applyControlsToBody(const MoveMsg& in, float dt);
    void applyLongitudinalForces(float throttle, float brake);
    void applySteering(float steer);
    void applyDrift();

    PlayerState snapshotState();

    ID getClientId() const {return this->clientId; };
    CarType getCarType() const {return this->carType; }
    float getHealth() const {return this->health; }
    ID getActualCheckpoint() const {return this->actualCheckpoint; };
    void setCheckpoint(ID checkpoint) { this->actualCheckpoint = checkpoint; }

    void applyDamage(const float damage);
    bool isCarDestroy();

    void markFinished(float t, u_int8_t finishedCarsCount) {
        finished = true;
        finishTime = t;
        ranking = finishedCarsCount;
    }
    bool isFinished() const { return finished; }
    float getFinishTime() const { return finishTime; }
    uint8_t getRanking() const { return ranking; }

private:
    ID clientId;
    CarType carType;
    float health = 100.0f;
    ID actualCheckpoint = 0;
    bool finished = false;
    float finishTime = 0.0f;
    uint8_t ranking = 0;
};

