#include "car.h"

#include <algorithm>
#include <cmath>

#include "../world_manager.h"

Car::Car(WorldManager& world,
         ID clientId,
         b2Vec2 pos,
         float angleRad,
         CarType carType, const CarHandlingConfig& carCongif)
        : Entity(EntityType::Car, b2_nullBodyId, 0),
          clientId(clientId), carType(carType), handling(carCongif)
{
    ID physId = world.createCarBody(pos, angleRad);
    this->setPhysicsId(physId);

    b2BodyId body = world.getBody(physId);
    this->setBody(body);

    auto* ud = new PhysicsUserData{
            PhysicsType::Car,
            clientId
    };
    b2Body_SetUserData(this->body, ud);
    resetRaceState(carCongif.baseHealth);
}

void Car::applyControlsToBody(const MoveMsg& in, float dt) {
    float t        = in.getAccelerate();
    float throttle = (t == 2) ? -1.0f : t;
    float steer    = in.getSteer();
    float brake    = in.getBrake();

    applyThrottle(throttle, dt);
    applyBrake(brake, dt);
    applySteering(steer);
    applyLateralFriction(dt);
}

void Car::applyThrottle(float throttle, float dt) {
    if (throttle == 0.0f) return;

    b2Rot rot = b2Body_GetRotation(body);
    b2Vec2 fwd = b2RotateVector(rot, (b2Vec2){0.f, 1.f});
    b2Vec2 vel = b2Body_GetLinearVelocity(body);

    float vLong = vel.x * fwd.x + vel.y * fwd.y;

    // hacia adelante
    if (throttle > 0.0f &&
        vLong < (handling.maxFwdSpeed *
                 raceState.maxSpeedFactor *
                 raceState.maxSpeedCheat)) {

        float impulse = handling.engineImpulse * raceState.engineFactor * dt;
        b2Vec2 j{ fwd.x * impulse, fwd.y * impulse };
        b2Body_ApplyLinearImpulseToCenter(body, j, true);
                 }

    // reversa
    if (throttle < 0.0f && vLong > handling.maxBckSpeed) {
        float impulse = handling.engineImpulse * dt;
        b2Vec2 j{ -fwd.x * impulse, -fwd.y * impulse };
        b2Body_ApplyLinearImpulseToCenter(body, j, true);
    }
}


void Car::applyBrake(float brake, float dt) {
    if (brake <= 0.0f) return;

    b2Vec2 curVel = b2Body_GetLinearVelocity(body);

    float speed = std::sqrt(curVel.x * curVel.x + curVel.y * curVel.y);
    if (speed <= 0.0f) return;

    float dv       = handling.brakeAccel * brake * dt;  // cuánto quiero bajar la speed este frame
    float newSpeed = std::max(0.0f, speed - dv);
    float factor   = newSpeed / speed;                  // entre 0 y 1

    curVel.x *= factor;
    curVel.y *= factor;
    b2Body_SetLinearVelocity(body, curVel);
}


void Car::applySteering(float steer) {
    float targetAV = steer * handling.maxAngularVel;
    b2Body_SetAngularVelocity(body, targetAV);
}

void Car::applyLateralFriction(float dt) {
    b2Rot rot   = b2Body_GetRotation(body);
    b2Vec2 fwd  = b2RotateVector(rot, (b2Vec2){0.f, 1.f});
    b2Vec2 right= b2RotateVector(rot, (b2Vec2){1.f, 0.f});
    b2Vec2 v    = b2Body_GetLinearVelocity(body);

    float vLong = v.x * fwd.x   + v.y * fwd.y;
    float vLat  = v.x * right.x + v.y * right.y;

    float factor = std::max(0.0f, 1.0f - handling.lateralDamp * dt);
    float vLatNew = vLat * factor;

    if (std::fabs(vLatNew) < 0.1f) {
        vLatNew = 0.0f;
    }

    b2Vec2 newVel{
        fwd.x * vLong + right.x * vLatNew,
        fwd.y * vLong + right.y * vLatNew
    };

    b2Body_SetLinearVelocity(body, newVel);
}

void Car::applyDamage(const float damage) {
    float newDamage = damage;
    newDamage = newDamage * raceState.shield;

    this->raceState.health -= newDamage;
    if (this->raceState.health <= 0) kill();
}

bool Car::applyUpgrade(const UpgradeDef& up) {
    bool apply = false;
    switch (up.type) {
        case Upgrade::ENGINE_FORCE: {
            if (raceState.engineFactor != 1.0f) break;
            raceState.engineFactor   = up.value;
            raceState.upgradePenalty += up.penaltySec;
            raceState.totalUpgrades  += 1;
            apply = true;
            break;
        }
        case HEALTH: {
            if (raceState.totalHealth != handling.baseHealth) break;
            raceState.health = raceState.health * up.value;
            raceState.totalHealth = raceState.health;
            raceState.upgradePenalty += up.penaltySec;
            raceState.totalUpgrades  += 1;
            apply = true;
            break;
        }
        case SHIELD: {
            if (raceState.shield != 1.0f) break;
            raceState.shield = up.value;
            raceState.upgradePenalty += up.penaltySec;
            raceState.totalUpgrades  += 1;
            apply = true;
            break;
        }
        case DAMAGE: {
            if (raceState.damage != 1.0f) break;
            raceState.damage = up.value;
            raceState.upgradePenalty += up.penaltySec;
            raceState.totalUpgrades  += 1;
            apply = true;
            break;
        }
        case Upgrade::NONE: {
            break;
        }
    }
    return apply;
}


void Car::applyCheat(const Cheat cheat) {
    switch (cheat) {
        case (Cheat::HEALTH_CHEAT): {
            raceState.health = CHEAT_HEALTH;
            raceState.totalHealth = CHEAT_HEALTH;
            break;
        }
        case (Cheat::FREE_SPEED_CHEAT): {
            raceState.maxSpeedCheat = 1000.f;
            break;
        }
        default: {
            break;
        }
    }
}

PlayerState Car::snapshotState(){
    b2Transform xf = b2Body_GetTransform(body);
    return PlayerState(clientId, xf.p.x, xf.p.y, b2Rot_GetAngle(xf.q));
}


void Car::resetForNewRace(float x, float y, float angleRad) {
    b2BodyId body = this->body;

    b2Vec2 pos{ x, y };
    b2Rot rot = b2MakeRot(angleRad);

    b2Body_Enable(body);

    b2Body_SetTransform(body, pos, rot);
    b2Body_SetLinearVelocity(body, {0.f, 0.f});
    b2Body_SetAngularVelocity(body, 0.f);

    resetRaceState(handling.baseHealth);
}

void Car::setPosition(float x, float y) {
    b2BodyId body = this->body;
    b2Vec2 pos{ x, y };
    b2Rot rot = b2Body_GetRotation(body);
    b2Body_SetTransform(body, pos, rot);
}

void Car::kill() {
    this->raceState.health = 0.f;
    //b2Body_Disable(body);
}

void Car::resetRaceState(float baseHealth) {
    raceState.health          = baseHealth;
    raceState.totalHealth     = baseHealth;
    raceState.actualCheckpoint= 0;
    raceState.finished        = false;
    raceState.finishTime      = 0.0f;
    raceState.ranking         = 0;
    raceState.spawnId         = 0;

    raceState.upgradePenalty  = 0.f;
    raceState.engineFactor    = 1.0f;
    raceState.maxSpeedFactor  = 1.0f;
    raceState.shield          = 1.0f;
    raceState.damage          = 1.0f;
    raceState.maxSpeedCheat   = 1.0f;
    raceState.totalUpgrades   = 0;
}