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
          clientId(clientId),
          carType(carType), handling(carCongif), health(handling.baseHealth)

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
}



void Car::applyControlsToBody(const MoveMsg& in, float dt) {
    float t = in.getAccelerate();
    float throttle = t;
    if (t == 2) throttle = -1;
    float steer    = in.getSteer();
    float brake    = in.getBrake();

    b2Rot rot = b2Body_GetRotation(body);
    b2Vec2 fwd   = b2RotateVector(rot, (b2Vec2){0.f, 1.f});
    b2Vec2 vel   = b2Body_GetLinearVelocity(body);

    float vLong = vel.x * fwd.x + vel.y * fwd.y;


    if (throttle > 0.0f && vLong < (handling.maxFwdSpeed * maxSpeedFactor * maxSpeedCheat)) {
        b2Vec2 j = { fwd.x * (handling.engineImpulse * engineFactor) * dt,
            fwd.y * handling.engineImpulse * engineFactor * dt };
        b2Body_ApplyLinearImpulseToCenter(body, j, true);
    }
    if (throttle < 0.0f && vLong > handling.maxBckSpeed) {
        b2Vec2 j = { -fwd.x * handling.engineImpulse * dt, -fwd.y * handling.engineImpulse * dt };
        b2Body_ApplyLinearImpulseToCenter(body, j, true);
    }

    if (brake > 0.0f) {
        b2Vec2 curVel = b2Body_GetLinearVelocity(body);

        float speed = std::sqrt(curVel.x * curVel.x + curVel.y * curVel.y);
        if (speed > 0.0f) {

            float dv = handling.brakeAccel * brake * dt;      // cuánto quiero bajar la speed este frame
            float newSpeed = std::max(0.0f, speed - dv);
            float factor = newSpeed / speed;          // entre 0 y 1

            curVel.x *= factor;
            curVel.y *= factor;
            b2Body_SetLinearVelocity(body, curVel);
        }
    }

    // giro
    float targetAV = steer * handling.maxAngularVel;
    b2Body_SetAngularVelocity(body, targetAV);

    // freno lateral para q derrape todo
    b2Rot rot2 = b2Body_GetRotation(body);
    b2Vec2 fwd2   = b2RotateVector(rot2, (b2Vec2){0.f, 1.f});
    b2Vec2 right2 = b2RotateVector(rot2, (b2Vec2){1.f, 0.f});
    b2Vec2 v      = b2Body_GetLinearVelocity(body);

    float vLong2 = v.x * fwd2.x   + v.y * fwd2.y;
    float vLat   = v.x * right2.x + v.y * right2.y;

    // frenamos laterlamente casi todo
    float factor = std::max(0.0f, 1.0f - handling.lateralDamp * dt);
    float vLatNew = vLat * factor;

    // para q el auto no quede temblando
    if (std::fabs(vLatNew) < 0.1f) {
        vLatNew = 0.0f;
    }

    b2Vec2 newVel = {
            fwd2.x * vLong2 + right2.x * vLatNew,
            fwd2.y * vLong2 + right2.y * vLatNew
    };

    b2Body_SetLinearVelocity(body, newVel);
}



void Car::applyDamage(const float damage) {
    float newDamage = damage;
    if (shield != 1) {
        newDamage = newDamage * shield;
    }
    this->health -= newDamage;
    if (this->health <= 0) kill();
}

void Car::kill() {
    this->health = 0.f;
    b2Body_Disable(body);   // o b2Body_SetEnabled(body, false) según versión
}

bool Car::isCarDestroy() {
    return this->health == 0;
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

    //tengo q hacer ya el struct
    health = 100.0f;
    actualCheckpoint = 0;
    finished         = false;
    finishTime       = 0.f;
    ranking          = 0;
    spawnId = 0;

    //habria q dejar comparar mejoras cada lobby (entre carreras)
    //upgradePenalty = 0.f;
    //upgrade = NONE;
    //engineFactor     = 1.0f;
    //maxSpeedFactor   = 1.0f;
    //shield   = 1.0f;
    //damage = 1.0f;
    //totalUpgrades = 0;
}




bool Car::applyUpgrade(const UpgradeDef& up) {
    bool apply = false;
    //std::cout << "[Car] applyUpgrade: type=" << (int)up.type
      //        << " totalUpgrades(before)=" << (int)totalUpgrades << "\n";

    switch (up.type) {
        case Upgrade::ENGINE_FORCE: {
            if (engineFactor != 1.0f) break; //para q no pueda elegir la misma devuelta
            engineFactor   = up.value;
            upgradePenalty += up.penaltySec;
            totalUpgrades  += 1;
            apply = true;
            break;
        }
        case HEALTH: {
            if (health > 100.0f) break;
            health = health * up.value;
            totalHealth += health;
            upgradePenalty += up.penaltySec;
            totalUpgrades  += 1;
            apply = true;
            break;
        }
        case SHIELD: {
            if (shield != 1.0f) break;
            shield = up.value;
            upgradePenalty += up.penaltySec;
            totalUpgrades  += 1;
            apply = true;
            break;
        }
        case DAMAGE: {
            if (damage != 1.0f) break;
            damage = up.value;
            upgradePenalty += up.penaltySec;
            totalUpgrades  += 1;
            apply = true;
            break;
        }
        case Upgrade::NONE: {
            break;
        }
    }

/*
    if (apply) {
        std::cout << "[Car]   APPLIED, totalUpgrades(after)="
                  << (int)totalUpgrades << "\n";
    } else {
        std::cout << "[Car]   NOT APPLIED\n";
    }
    */
    return apply;
}



float Car::getDamage() {
    return damage;
}


void Car::setPosition(float x, float y) {
    b2BodyId body = this->body;
    b2Vec2 pos{ x, y };
    b2Rot rot = b2Body_GetRotation(body);
    b2Body_SetTransform(body, pos, rot);
}


void Car::applyCheat(const Cheat cheat) {
    switch (cheat) {
        case (Cheat::HEALTH_CHEAT): {
            health = CHEAT_HEALTH;
            totalHealth = CHEAT_HEALTH;
            break;
        }
        case (Cheat::FREE_SPEED_CHEAT): {
            maxSpeedCheat = 1000.f;
            break;
        }
        default: {
            break;
        }
    }
}

b2Vec2 Car::getPosition() const {
    b2Transform xf = b2Body_GetTransform(body);
    return xf.p;   // (x, y) en el mundo
}

float Car::getAngleRad() const {
    b2Transform xf = b2Body_GetTransform(body);
    return b2Rot_GetAngle(xf.q);  // ángulo en radianes
}