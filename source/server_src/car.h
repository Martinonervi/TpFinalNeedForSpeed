#pragma once
#include <box2d/box2d.h>
#include "../common_src/constants.h"
#include "../common_src/move_Info.h"
#include "../common_src/player_state.h"
#include "entity.h"


class Car : public Entity {
public:
    Car(WorldManager& world,
        ID clientId,
        b2Vec2 pos,
        float angleRad,
        CarType carType);

    void applyControlsToBody(const MoveMsg& in, float dt);
    void applyControlsToBodyy(const MoveMsg& in, float dt);
    PlayerState snapshotState();

    ID getClientId() const {return this->clientId; };
    CarType getCarType() const {return this->carType; }
    float getHealth() const {return this->health; }
    ID getActualCheckpoint() const {return this->actualCheckpoint; };
    void setCheckpoint(ID checkpoint) { this->actualCheckpoint = checkpoint; }

    void applyDamage(const float damage);
    bool isCarDestroy();

private:
    ID clientId;
    CarType carType;
    float health;
    ID actualCheckpoint = 0;

};

