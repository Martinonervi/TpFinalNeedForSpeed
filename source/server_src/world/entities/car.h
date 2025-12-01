#pragma once
#include <box2d/box2d.h>

#include "../../../common_src/cli_msg/move_Info.h"
#include "../../../common_src/cli_msg/cli_cheat_request.h"
#include "../../../common_src/constants.h"
#include "../../../common_src/srv_msg/player_state.h"

#include "entity.h"
#include "../../game_logic/config/config_parser.h"

constexpr int CAR_MAX_UPGRADES = 3;
constexpr float CHEAT_HEALTH = 10000.0f;

struct RaceState {
    float health = 0.0f;
    ID actualCheckpoint = 0;
    bool finished = false;
    float finishTime = 0.0f;
    uint8_t ranking = 0;
    ID spawnId = 0;

    float upgradePenalty = 0.f;
    float engineFactor   = 1.0f;
    float maxSpeedFactor = 1.0f;
    float shield         = 1.0f;
    float damage         = 1.0f;
    float maxSpeedCheat = 1.0f;
    uint8_t totalUpgrades = 0;
    float totalHealth = 0.0f;
};


class Car : public Entity {
public:
    Car(WorldManager& world,
        ID clientId,
        b2Vec2 pos,
        float angleRad,
        CarType carType,
        const CarHandlingConfig& carCongif);

    void applyControlsToBody(const MoveMsg& in, float dt);
    PlayerState snapshotState();

    bool applyUpgrade(const UpgradeDef& up);
    void applyCheat(Cheat cheat);
    void applyDamage(float damage);
    void resetForNewRace(float x, float y, float angleDeg);
    void kill();
    void markFinished(float t, uint8_t finishedCarsCount) {
        raceState.finished = true;
        raceState.finishTime = t;
        raceState.ranking = finishedCarsCount;
    }

    // booleanos
    bool isCarDestroy() const {
        return this->raceState.health == 0;
    }
    bool isFinished() const { return raceState.finished; }
    bool hasMaxUpgrade() const {return raceState.totalUpgrades >= CAR_MAX_UPGRADES;}


    // getters
    ID getClientId() const {return this->clientId; };
    CarType getCarType() const {return this->carType; }
    float getHealth() const {return this->raceState.health; }
    ID getActualCheckpoint() const {return this->raceState.actualCheckpoint; };
    float getFinishTime() const { return raceState.finishTime + raceState.upgradePenalty; }
    float getFinishTimeNoPenalty() const { return raceState.finishTime; }
    uint8_t getRanking() const { return raceState.ranking; }
    ID getSpawnId() const { return raceState.spawnId; }
    b2Vec2 getPosition() const {
        b2Transform xf = b2Body_GetTransform(body);
        return xf.p;
    }
    float getAngleRad() const {
        b2Transform xf = b2Body_GetTransform(body);
        return b2Rot_GetAngle(xf.q);
    }

    float getPenalty() const {
        return raceState.upgradePenalty;
    };
    float getTotalHealth() const {
        return raceState.totalHealth;
    }
    float getShield() const {return raceState.shield;}
    float getDamage() const {return raceState.damage;}

    // setters
    void setCheckpoint(ID checkpoint) { this->raceState.actualCheckpoint = checkpoint; }
    void setPosition(float x, float y);

private:
    ID clientId;
    CarType carType;
    CarHandlingConfig handling;
    RaceState raceState;

    void resetRaceState(float baseHealth);

    // helpers de applyControlsToBody
    void applyThrottle(float throttle, float dt);
    void applyBrake(float brake, float dt);
    void applySteering(float steer);
    void applyLateralFriction(float dt);

};

