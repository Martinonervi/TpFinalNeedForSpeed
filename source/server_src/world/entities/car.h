#pragma once
#include <box2d/box2d.h>

#include "../../../common_src/cli_msg/move_Info.h"
#include "../../../common_src/cli_msg/cli_cheat_request.h"
#include "../../../common_src/constants.h"
#include "../../../common_src/srv_msg/player_state.h"

#include "entity.h"

const int CAR_MAX_UPGRADES = 3;

struct RaceState {
    float health = 100.0f;
    ID actualCheckpoint = 0;
    bool finished = false;
    float finishTime = 0.0f;
    uint8_t ranking = 0;
    ID spawnId = 0;
    float speed = 0;

    float upgradePenalty = 0.f;
    Upgrade upgrade = NONE;
    float engineFactor   = 1.0f;
    float maxSpeedFactor = 1.0f;
    float shield         = 1.0f;
    float damage         = 1.0f;
};

class Car : public Entity {
public:
    Car(WorldManager& world,
        ID clientId,
        b2Vec2 pos,
        float angleRad,
        CarType carType);

    void applyControlsToBody(const MoveMsg& in, float dt);

    PlayerState snapshotState();
    bool applyUpgrade(const UpgradeDef& up);

    ID getClientId() const {return this->clientId; };
    CarType getCarType() const {return this->carType; }
    float getHealth() const {return this->health; }
    ID getActualCheckpoint() const {return this->actualCheckpoint; };
    void setCheckpoint(ID checkpoint) { this->actualCheckpoint = checkpoint; }

    void applyDamage(const float damage);
    void kill();
    bool isCarDestroy();

    void markFinished(float t, u_int8_t finishedCarsCount) {
        finished = true;
        finishTime = t;
        ranking = finishedCarsCount;
    }
    bool isFinished() const { return finished; }
    float getFinishTime() const { return finishTime + upgradePenalty; }
    uint8_t getRanking() const { return ranking; }

    void resetForNewRace(float x, float y, float angleDeg);

    ID getSpawnId() const { return spawnId; }
    bool hasMaxUpgrade() const {
        //std::cout << "[Car] hasMaxUpgrade? total=" << (int)totalUpgrades
          //    << " max=" << CAR_MAX_UPGRADES << "\n";
              return totalUpgrades >= CAR_MAX_UPGRADES;
    }

    float getShield();
    float getDamage();
    void setPosition(float x, float y);

    void applyCheat(Cheat cheat);


private:
    ID clientId;
    CarType carType;

    float health = 100.0f;
    ID actualCheckpoint = 0;
    bool finished = false;
    float finishTime = 0.0f;
    uint8_t ranking = 0;
    ID spawnId = 0;

    // logica de mejoras
    // factores de mejora (empiezan en 1.0 = sin mejora)
    float upgradePenalty = 0.f;
    Upgrade upgrade = NONE;
    float engineFactor     = 1.0f;
    float maxSpeedFactor   = 1.0f;
    float shield   = 1.0f;
    float damage = 1.0f;
    uint8_t totalUpgrades = 0;

    float maxSpeedCheat = 1.0f;

};

