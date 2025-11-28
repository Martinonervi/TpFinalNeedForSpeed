#include "car.h"

#include <algorithm>
#include <cmath>

#include "../world_manager.h"

Car::Car(WorldManager& world,
         ID clientId,
         b2Vec2 pos,
         float angleRad,
         CarType carType)
        : Entity(EntityType::Car, b2_nullBodyId, 0),
          clientId(clientId),
          carType(carType)

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

// longitudinal
const float MAX_FWD_SPEED  = 50.0f;
const float MAX_BCK_SPEED  = -10.0f;
const float ENGINE_IMPULSE = 800.0f;

// lateral
const float BRAKE_ACCEL    = 100.0f;
const float MAX_ANGULAR_VEL = 2.5f; // podria probar 2.5–3.5
const float LATERAL_DAMP    = 9.0f; // probá 6–12


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


    if (throttle > 0.0f && vLong < (MAX_FWD_SPEED * maxSpeedFactor)) {
        b2Vec2 j = { fwd.x * (ENGINE_IMPULSE * engineFactor) * dt, fwd.y * ENGINE_IMPULSE * dt };
        b2Body_ApplyLinearImpulseToCenter(body, j, true);
    }
    if (throttle < 0.0f && vLong > MAX_BCK_SPEED) {
        b2Vec2 j = { -fwd.x * ENGINE_IMPULSE * dt, -fwd.y * ENGINE_IMPULSE * dt };
        b2Body_ApplyLinearImpulseToCenter(body, j, true);
    }

    if (brake > 0.0f) {
        b2Vec2 curVel = b2Body_GetLinearVelocity(body);

        float speed = std::sqrt(curVel.x * curVel.x + curVel.y * curVel.y);
        if (speed > 0.0f) {

            float dv = BRAKE_ACCEL * brake * dt;      // cuánto quiero bajar la speed este frame
            float newSpeed = std::max(0.0f, speed - dv);
            float factor = newSpeed / speed;          // entre 0 y 1

            curVel.x *= factor;
            curVel.y *= factor;
            b2Body_SetLinearVelocity(body, curVel);
        }
    }

    // giro
    float targetAV = steer * MAX_ANGULAR_VEL;
    b2Body_SetAngularVelocity(body, targetAV);

    // freno lateral para q derrape todo
    b2Rot rot2 = b2Body_GetRotation(body);
    b2Vec2 fwd2   = b2RotateVector(rot2, (b2Vec2){0.f, 1.f});
    b2Vec2 right2 = b2RotateVector(rot2, (b2Vec2){1.f, 0.f});
    b2Vec2 v      = b2Body_GetLinearVelocity(body);

    float vLong2 = v.x * fwd2.x   + v.y * fwd2.y;
    float vLat   = v.x * right2.x + v.y * right2.y;

    // frenamos laterlamente casi todo
    float factor = std::max(0.0f, 1.0f - LATERAL_DAMP * dt);
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
    //multa = 0
    spawnId = 0;
    upgradePenalty = 0.f;
    upgrade = NONE;
    engineFactor     = 1.0f;
    maxSpeedFactor   = 1.0f;
    shield   = 1.0f;
    damage = 1.0f;
    totalUpgrades = 0;
}




void Car::applyUpgrade(const UpgradeDef& up) {
    switch (up.type) {
        case Upgrade::ENGINE_FORCE: {
            if (engineFactor != 1.0f) break; //para q no pueda elegir la misma devuelta
            engineFactor   = up.value;
            upgradePenalty += up.penaltySec;
            totalUpgrades  += 1;
            break;
        }
        case HEALTH: {
            if (health != 1.0f) break;
            health = health * up.value;
            upgradePenalty += up.penaltySec;
            totalUpgrades  += 1;
            break;
        }
        case SHIELD: {
            if (shield != 1.0f) break;
            shield = up.value;
            upgradePenalty += up.penaltySec;
            totalUpgrades  += 1;
            break;
        }
        case DAMAGE: {
            if (damage != 1.0f) break;
            damage = up.value;
            upgradePenalty += up.penaltySec;
            totalUpgrades  += 1;
            break;
        }
        case Upgrade::NONE: {
            break;
        }
    }
}



float Car::getDamage() {
    return damage;
}
