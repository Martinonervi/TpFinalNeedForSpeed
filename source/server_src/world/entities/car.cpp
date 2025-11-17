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

namespace {
constexpr float ENGINE_FWD  = 1500.f;
constexpr float ENGINE_REV  = 1200.f;
constexpr float BRAKE_FORCE = 3600.f;
constexpr float DRAG_K      = 10.f;

constexpr float VMAX_FWD    = 26.f;
constexpr float VMAX_REV    = 9.f;

constexpr float SPEED_REF   = 25.f;
constexpr float MAX_TORQUE  = 140.f;
}

void Car::applyControlsToBody(const MoveMsg& in, float /*dt*/) {
    const uint8_t acc = in.getAccelerate();
    const float throttle = (acc == 1) ? 1.f : (acc == 2) ? -1.f : 0.f;
    const float brake    = in.getBrake() ? 1.f : 0.f;
    float steer          = in.getSteer();

    applyLongitudinalForces(throttle, brake);
    applySteering(steer);
    applyDrift();
}


void Car::applyLongitudinalForces(float throttle, float brake) {
    b2Rot  rot = b2Body_GetRotation(body);
    b2Vec2 fwd = b2RotateVector(rot, {0.f, 1.f});
    b2Vec2 vel = b2Body_GetLinearVelocity(body);

    float v_long = vel.x * fwd.x + vel.y * fwd.y;

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
}


namespace {
constexpr float TURN_SPEED = 2.8f;   // rad/s, qué tan rápido gira a fondo
constexpr float STEER_SMOOTH = 0.25f; // cuánto “suavizás” el cambio de omega (0–1)
}

void Car::applySteering(float steer) {
    // steer ∈ {-1, 0, 1} o [-1,1], da igual
    if (steer == 0.f) {
        // si querés que deje de girar cuando soltás
        float currentOmega = b2Body_GetAngularVelocity(body);
        float newOmega = lerp(currentOmega, 0.f, STEER_SMOOTH);
        b2Body_SetAngularVelocity(body, newOmega);
        return;
    }

    float steerNorm = clampf(steer, -1.f, 1.f);

    float targetOmega = steerNorm * TURN_SPEED;
    float currentOmega = b2Body_GetAngularVelocity(body);

    // suavizo el cambio para que no sea brusco
    float newOmega = lerp(currentOmega, targetOmega, STEER_SMOOTH);

    b2Body_SetAngularVelocity(body, newOmega);
}

namespace {
constexpr float DRIFT_SPEED_REF = 25.f; // a partir de acá decimos "va rápido"
constexpr float LATERAL_K_SLOW  = 0.8f; // cuánto mato la vel. lateral cuando va lento
constexpr float LATERAL_K_FAST  = 0.4f; // cuánto mato la vel. lateral cuando va rápido
}

void Car::applyDrift() {
    b2Vec2 vel = b2Body_GetLinearVelocity(body);
    float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

    if (speed < 0.1f) return;

    b2Rot  rot = b2Body_GetRotation(body);
    b2Vec2 fwd = b2RotateVector(rot, {0.f, 1.f});
    b2Vec2 right{ fwd.y, -fwd.x }; // perpendicular


    float v_fwd    = vel.x * fwd.x   + vel.y * fwd.y;
    float v_lat    = vel.x * right.x + vel.y * right.y;

    // cuánto quiero matar v_lat según la velocidad
    float speedFactor = clampf(speed / DRIFT_SPEED_REF, 0.f, 1.f);
    float k = lerp(LATERAL_K_SLOW, LATERAL_K_FAST, speedFactor);
    // k ~ 0.8 en lento (mato casi todo), k ~ 0.4 en rápido (dejo más drift)

    float v_lat_new = v_lat * (1.f - k); // reduzco la parte lateral

    // reconstruyo la velocidad
    b2Vec2 newVel =
            v_fwd * fwd +
            v_lat_new * right;

    b2Body_SetLinearVelocity(body, newVel);
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