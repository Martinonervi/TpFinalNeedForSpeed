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

static inline float clampf(float x, float a, float b){ return std::max(a, std::min(b, x)); }

static inline float lerp(float a, float b, float t) {
    t = clampf(t, 0.f, 1.f);
    return a + (b - a) * t;
}

void Car::applyControlsToBody(const MoveMsg& in, float dt) {
    const uint8_t acc = in.getAccelerate();
    const float throttle = (acc == 1) ? 1.f : (acc == 2) ? -1.f : 0.f;
    const float brake    = in.getBrake() ? 1.f : 0.f;
    float steer          = in.getSteer();

    b2Rot  rot = b2Body_GetRotation(body);
    b2Vec2 fwd = b2RotateVector(rot, {0.f, 1.f});
    b2Vec2 vel = b2Body_GetLinearVelocity(body);

    float v_long = vel.x * fwd.x + vel.y * fwd.y;

    const float ENGINE_FWD  = 1500.f;
    const float ENGINE_REV  = 1200.f;
    const float BRAKE_FORCE = 3600.f;
    const float DRAG_K      = 10.f;

    const float VMAX_FWD = 26.f;
    const float VMAX_REV = 9.f;
    float vmax = (v_long >= 0.f) ? VMAX_FWD : VMAX_REV;

    b2Vec2 F{0.f, 0.f};

    if (throttle != 0.f) {
        float Eng = (throttle > 0.f) ? ENGINE_FWD : ENGINE_REV;
        float engine_cap = 1.f - clampf(std::fabs(v_long) / vmax, 0.f, 1.f);
        F += (Eng * throttle * engine_cap) * fwd;
    }

    if (brake > 0.f) {
        float dir = (v_long >= 0.f) ? -1.f : 1.f;
        F += dir * (BRAKE_FORCE * brake) * fwd;
    }

    F += (-DRAG_K * v_long) * fwd;

    b2Body_ApplyForceToCenter(body, F, true);

    if (steer != 0.f) {
        float steerNorm = clampf(steer, -1.f, 1.f);

        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
        const float SPEED_REF = 25.f;

        float speedFactor  = 1.f - clampf(speed / SPEED_REF, 0.f, 1.f);
        float torqueScale  = 0.3f + 0.7f * speedFactor;

        const float MAX_TORQUE = 140.f;
        float torque = steerNorm * MAX_TORQUE * torqueScale;

        b2Body_ApplyTorque(body, torque, true);



        rot = b2Body_GetRotation(body);
        fwd = b2RotateVector(rot, {0.f, 1.f});
        vel = b2Body_GetLinearVelocity(body);

        float speedNow = std::sqrt(vel.x * vel.x + vel.y * vel.y);

        b2Vec2 targetVel = { fwd.x * speedNow, fwd.y * speedNow };

        float alignBase = 0.45f;
        float align = alignBase * std::fabs(steerNorm);

        b2Vec2 newVel {
            lerp(vel.x, targetVel.x, align),
            lerp(vel.y, targetVel.y, align)
        };
        b2Body_SetLinearVelocity(body, newVel);
    }
}


void Car::applyControlsToBodyyy(const MoveMsg& in, float dt) {
    float throttle = in.getAccelerate();
    if (throttle == 2) {
        throttle = -1.0f;
    }
    const float brake    = clampf(in.getBrake(),      0.f, 1.f);
    const float steer    = clampf(in.getSteer(),     -1.f, 1.f);


    b2Rot rot = b2Body_GetRotation(body);
    b2Vec2 fwd   = b2RotateVector(rot, {0.f, 1.f});
    b2Vec2 right = b2RotateVector(rot, {1.f, 0.f});


    b2Vec2 vel = b2Body_GetLinearVelocity(body);
    float v_long = vel.x * fwd.x + vel.y * fwd.y;
    float v_lat  = vel.x * right.x + vel.y * right.y;


    const float engineForce = 6000.f;
    const float brakeForce  = 1200.f;
    const float c_drag      = 12.f;
    const float maxSpeed    = 200.f;

    b2Vec2 F = {0.f, 0.f};

    if (throttle != 0.f && std::fabs(v_long) < maxSpeed * 1.01f) {
        float scale = 1.f - clampf(std::fabs(v_long) / maxSpeed, 0.f, 1.f);
        F += engineForce * throttle * scale * fwd;
    }

    if (brake > 0.f) {
        float dir = (v_long >= 0.f) ? -1.f : 1.f;
        F += dir * brakeForce * brake * fwd;
    }


    F += (-c_drag * v_long) * fwd;


    b2Body_ApplyForceToCenter(body, F, true);

    float m = b2Body_GetMass(body);
    float slipFactor = 0.75f;
    b2Vec2 J = (-m * v_lat * slipFactor * right);
    b2Body_ApplyLinearImpulseToCenter(body, J, true);

    float angle = std::atan2(rot.s, rot.c);

    const float PI = 3.14159265f;
    const float turnSpeed = PI / 2.0f;
    float turn = steer * turnSpeed * dt;

    float newAngle = angle + turn;

    b2Vec2 pos = b2Body_GetPosition(body);
    b2Body_SetTransform(body, pos, b2MakeRot(newAngle));

    b2Vec2 newFwd = b2RotateVector(b2MakeRot(newAngle), {0.f, 1.f});

    // proyectamos la velocidad actual sobre la nueva dirección
    float speedAlongNewFwd = vel.x * newFwd.x + vel.y * newFwd.y;

    // armamos una velocidad nueva hacia adelante
    b2Vec2 newVel = { newFwd.x * speedAlongNewFwd, newFwd.y * speedAlongNewFwd };
    b2Body_SetLinearVelocity(body, newVel);

    const float stopEps = 0.05f;
    if (std::fabs(speedAlongNewFwd) < stopEps && throttle == 0.f && brake == 1.f) {
        b2Vec2 v2 = b2Body_GetLinearVelocity(body);
        if (std::fabs(v2.x) < 0.02f && std::fabs(v2.y) < 0.02f) {
            b2Body_SetLinearVelocity(body, {0.f, 0.f});
        }
    }
}



void Car::applyDamage(const float damage) {
    this->health -= damage;
    if (this->health < 0) this->health = 0;
}

bool Car::isCarDestroy() {
    return this->health == 0;
}


PlayerState Car::snapshotState(){
    b2Transform xf = b2Body_GetTransform(body);
    return PlayerState(clientId, xf.p.x, xf.p.y, b2Rot_GetAngle(xf.q));
}