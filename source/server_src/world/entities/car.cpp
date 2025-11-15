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
static inline float lerp(float a, float b, float t){ return a + (b - a) * clampf(t, 0.f, 1.f); }
static inline float smooth01(float x){ x = clampf(x,0.f,1.f); return x*x*(3.f-2.f*x); }
static inline int sgnz(float x){ return (x>0.f) - (x<0.f); }

struct CarTuning {
    // Longitudinal
    float ENGINE_FWD   = 1500.f;
    float ENGINE_REV   = 1200.f;
    float BRAKE_FORCE  = 3600.f;
    float DRAG_K       = 10.0f;

    // Vel máx
    float VMAX_FWD     = 26.f;
    float VMAX_REV     = 9.f;
    float VMAX_OVERS   = 0.01f;
    float VCAP_IMP     = 0.22f;

    // Alta velocidad
    float HIGH_SPEED_TH    = 10.0f; // m/s
    float TAP_GRACE_HIGH   = 0.10f; // s
    float TAP_GRACE_LOW    = 0.06f; // s

    // Centro/steer
    float STEER_DEADZONE   = 0.03f;
    float CENTER_EPS       = 0.10f;
    float CENTER_EXP       = 1.4f;
    float STEER_EMA_ALPHA  = 0.30f;

    // Acumulación (hold)
    float INTENT_RISE      = 2.4f;
    float INTENT_FALL      = 1.2f;
    float INTENT_POW       = 1.35f;

    // Giro
    float LOW_TURN         = 2.6f;
    float HIGH_TURN        = 1.0f;
    float SPEED_FADE_K     = 0.020f;
    float TURN_RATE_MAX    = 3.2f;
    float MIN_TURN_BASE    = 0.25f;

    // Alinear vel→heading (siempre acompaña la imagen)
    float ALIGN_BASE       = 0.50f;
    float ALIGN_STEER_GAIN = 0.35f;
    float ALIGN_SPEED_K    = 0.010f;
    float ALIGN_MAX        = 0.95f;
    float ALIGN_MIN_TAP    = 0.10f;

    // Parada / anti-creep
    float STOP_EPS_L       = 0.05f;
    float STOP_EPS_V       = 0.03f;
    float SNAP_SPEED       = 0.15f;
};

static CarTuning T;

struct SteeringState {
    float steerEma = 0.f;
    float intent   = 0.f;
    int   holdDir  = 0;
    float holdTime = 0.f;
};
static SteeringState S;

//tengo que ver esto con detalle
void Car::applyControlsToBody(const MoveMsg& in, float dt) {
    if (dt <= 0.f) dt = 1.f/60.f;


    const uint8_t acc = in.getAccelerate();
    const float throttle = (acc==1)? 1.f : (acc==2)? -1.f : 0.f;
    const float brake    = (in.getBrake()? 1.f : 0.f);
    float steerRaw       = clampf(static_cast<float>(in.getSteer()), -1.f, 1.f);

    if (std::fabs(steerRaw) < T.STEER_DEADZONE) steerRaw = 0.f;
    S.steerEma = lerp(S.steerEma, steerRaw, T.STEER_EMA_ALPHA);
    const float steer = clampf(S.steerEma, -1.f, 1.f);

    const b2Rot  rot   = b2Body_GetRotation(body);
    const b2Vec2 fwd   = b2RotateVector(rot, {0.f, 1.f});
    const b2Vec2 right = b2RotateVector(rot, {1.f, 0.f});
    const b2Vec2 vel   = b2Body_GetLinearVelocity(body);
    const float  mass  = b2Body_GetMass(body);

    const float v_long = vel.x * fwd.x + vel.y * fwd.y;
    const float speed  = std::sqrt(vel.x*vel.x + vel.y*vel.y);
    const float vmax   = (v_long >= 0.f) ? T.VMAX_FWD : T.VMAX_REV;

    {
        b2Vec2 F{0.f, 0.f};
        const float engine_cap = 1.f - clampf(std::fabs(v_long)/vmax, 0.f, 1.f);
        if (throttle != 0.f) {
            const float Eng = (throttle > 0.f) ? T.ENGINE_FWD : T.ENGINE_REV;
            F += (Eng * throttle * engine_cap) * fwd;
        }
        if (brake > 0.f) {
            const float dir = (v_long >= 0.f) ? -1.f : 1.f;
            F += dir * (T.BRAKE_FORCE * brake) * fwd;
        }
        F += (-T.DRAG_K * v_long) * fwd;
        b2Body_ApplyForceToCenter(body, F, true);
    }


    const bool  highSpeed = std::fabs(v_long) >= T.HIGH_SPEED_TH;
    const float tapGrace  = highSpeed ? T.TAP_GRACE_HIGH : T.TAP_GRACE_LOW;

    const int dir = sgnz(steer);
    if (dir == 0) {
        S.holdDir = 0;
        S.holdTime = 0.f;
        S.intent = clampf(S.intent - T.INTENT_FALL * dt, 0.f, 1.f);
    } else {
        if (dir != S.holdDir) {
            S.holdDir = dir;
            S.holdTime = 0.f;
            S.intent *= 0.5f;
        } else {
            S.holdTime += dt;
        }
        const float mag = std::pow(std::fabs(steer), T.INTENT_POW);
        if (S.holdTime >= tapGrace) {
            S.intent = clampf(S.intent + (T.INTENT_RISE * mag) * dt, 0.f, 1.f);
        }
    }

    float centerX = clampf(std::fabs(steer) / std::max(1e-3f, T.CENTER_EPS), 0.f, 1.f);
    centerX = std::pow(centerX, T.CENTER_EXP);
    const float centerScale = smooth01(centerX);

    const float tapScale = (S.holdTime >= tapGrace) ? 1.f : 0.f;

    float baseTurnBySpeed = lerp(T.HIGH_TURN, T.LOW_TURN,
                                 clampf((T.HIGH_SPEED_TH - std::fabs(v_long))/T.HIGH_SPEED_TH, 0.f, 1.f));
    float baseTurnByIntent = lerp(T.LOW_TURN, T.HIGH_TURN, S.intent);
    float baseTurn = std::max(baseTurnBySpeed, baseTurnByIntent);

    const float speedFade = 1.f / (1.f + T.SPEED_FADE_K * std::fabs(v_long));

    float turnRate = baseTurn * speedFade * centerScale * tapScale;
    turnRate = clampf(turnRate, 0.f, T.TURN_RATE_MAX);


    if (std::fabs(steer) > 0.f) {
        const float minTurn = T.MIN_TURN_BASE * (1.f / (1.f + 0.02f * std::fabs(v_long)));
        turnRate = std::max(turnRate, minTurn);
    }


    const float angle    = std::atan2(rot.s, rot.c);
    const float dtheta   = steer * turnRate * dt;
    const float newAngle = angle + dtheta;

    const b2Vec2 pos     = b2Body_GetPosition(body);
    const b2Rot  newRot  = b2MakeRot(newAngle);
    b2Body_SetTransform(body, pos, newRot);

    const b2Vec2 newFwd    = b2RotateVector(newRot, {0.f, 1.f});
    const b2Vec2 targetVel = { newFwd.x * speed, newFwd.y * speed };

    float alignK = T.ALIGN_BASE + T.ALIGN_STEER_GAIN * std::fabs(steer);
    alignK *= 1.f / (1.f + T.ALIGN_SPEED_K * std::fabs(v_long));
    alignK *= tapScale; // por qué: en tap puro evitá reorientar en exceso

    const float alignMin = T.ALIGN_MIN_TAP / (1.f + T.ALIGN_SPEED_K * std::fabs(v_long));
    alignK = std::max(alignK, alignMin);
    alignK = clampf(alignK, 0.f, T.ALIGN_MAX);

    const b2Vec2 newVel = { lerp(vel.x, targetVel.x, alignK),
                            lerp(vel.y, targetVel.y, alignK) };
    b2Body_SetLinearVelocity(body, newVel);

    b2Body_SetAngularVelocity(body, 0.f);

    if (std::fabs(v_long) > vmax * (1.f + T.VMAX_OVERS)) {
        const float excess = std::fabs(v_long) - vmax * (1.f + T.VMAX_OVERS);
        const float s      = (v_long >= 0.f) ? 1.f : -1.f;
        const b2Vec2 jcap  = (-mass * excess * T.VCAP_IMP * s) * fwd;
        b2Body_ApplyLinearImpulseToCenter(body, jcap, true);
    }

    if (std::fabs(v_long) < T.STOP_EPS_L && brake > 0.9f && throttle == 0.f) {
        const b2Vec2 v2 = b2Body_GetLinearVelocity(body);
        if (std::fabs(v2.x) < T.STOP_EPS_V && std::fabs(v2.y) < T.STOP_EPS_V) {
            b2Body_SetLinearVelocity(body, {0.f, 0.f});
            b2Body_SetAngularVelocity(body, 0.f);
        }
    }
    if (speed < T.SNAP_SPEED) {
        const b2Vec2 vSnap = { fwd.x * v_long, fwd.y * v_long };
        b2Body_SetLinearVelocity(body, vSnap);
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

    float angle = std::atan2(rot.s, rot.c);  // rot = {c, s}

    // cuánto queremos girar este frame
    const float PI = 3.14159265f;
    const float turnSpeed = PI / 2.0f;        // rad/s
    float turn = steer * turnSpeed * dt;         // lo que giro en este frame

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