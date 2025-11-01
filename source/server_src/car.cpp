#include "car.h"
#include <algorithm>
#include <cmath>

Car::Car(ID id, b2WorldId world, b2Vec2 pos, float angleRad){
    this->id = id;

    // defino body
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = pos;
    bd.rotation = b2MakeRot(angleRad);
    bd.linearDamping = 2.0f;
    bd.angularDamping = 3.0f;

    // creo body
    body = b2CreateBody(world, &bd);

    //creo shape
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = 1.0f;
    sd.material.friction = 0.3f;
    sd.material.restitution = 0.08f;
    b2Polygon box = b2MakeBox(1.0f, 2.0f); //rectangulo 2x4

    // creo fixture
    b2CreatePolygonShape(body, &sd, &box);
}


PlayerState Car::snapshotState(){
    b2Transform xf = b2Body_GetTransform(body);
    return PlayerState(id, xf.p.x, xf.p.y, b2Rot_GetAngle(xf.q));
}


static inline float clamp(float x, float a, float b){
    return std::max(a, std::min(b, x));
}


const float aLong       = 10.0f;   // m/s² aceleración máxima
const float dBrake      = 14.0f;   // m/s² desaceleración de freno
const float dragCoeff   = 0.8f;    // 1/s  arrastre lineal
const float maxSpeed    = 25.0f;   // m/s  tope de velocidad
const float turnRateMax = 20.5f;    // rad/s giro máximo
const float stopEps     = 0.05f;   // m/s  umbral para “snap to zero”


// Convención pedida: eje X local = DERECHA, eje Y local = ADELANTE
static inline float clampf(float x, float a, float b){ return std::max(a, std::min(b, x)); }

void Car::applyControlsToBody(const MoveMsg& in, float dt) {
    b2BodyId body = this->body;

    // Inputs
    const float throttle = clampf(in.getAccelerate(), 0.f, 1.f);
    const float brake    = clampf(in.getBrake(), 0.f, 1.f);
    const float steer    = clampf(in.getSteer(), -1.f, 1.f);


    //  (X = derecha, Y = adelante) -> hablar con fran
    b2Rot rot   = b2Body_GetRotation(body);
    b2Vec2 right = b2RotateVector(rot, (b2Vec2){1.f, 0.f});
    b2Vec2 fwd   = b2RotateVector(rot, (b2Vec2){0.f, 1.f});

    // Velocidad eje x
    b2Vec2 v = b2Body_GetLinearVelocity(body);
    float v_long = v.x * fwd.x + v.y * fwd.y;

    // Aceleración
    float acc = aLong * throttle;

    // Freno
    if (brake > 0.f) {
        float sign = (v_long > 0.f) ? 1.f : (v_long < 0.f ? -1.f : 0.f);
        acc -= dBrake * brake * sign;
    }

    // perder aceleracion de a poco?
    acc -= dragCoeff * v_long;

    // le sumamos a la velocidad lo ganado por la aceleracion
    v_long += acc * dt;

    // Ver si voy tan lento, tengo q frenar
    if (std::fabs(v_long) < stopEps && brake > 0.5f) v_long = 0.f;

    // Límite de velocidad
    v_long = clampf(v_long, -maxSpeed, maxSpeed);

    // Por ahora ignoro el eje y
    float v_lat = 0.0f;

    // Recomponer velocidad en mundo y setear
    b2Vec2 vNew = {
            fwd.x * v_long + right.x * v_lat,
            fwd.y * v_long + right.y * v_lat
    };
    b2Body_SetLinearVelocity(body, vNew);

    // velocidad angular
    float speedNorm = clampf(std::fabs(v_long) / maxSpeed, 0.f, 1.f);
    float omega = steer * (0.2f + 0.8f * speedNorm) * turnRateMax;
    b2Body_SetAngularVelocity(body, omega);
}


/* muy complicado, mejor arrancar con algo simple
void Car::applyControlsToBody(Car& car, const MoveInfo& in, float dt) {
    b2BodyId body = car.body;

    // Inputs
    const float throttle = clamp(in.accelerate, 0.f, 1.f); // [0,1]
    const float brake    = clamp(in.brake,      0.f,  1.f); // [0,1]
    const float steer    = clamp(in.steer,     -1.f,  1.f); // [-1,1]

    //devuelve struct con p (posicion, vec, pos en m) y q (rotacion)
    const b2Transform xf = b2Body_GetTransform(body);

    const float ang = b2Rot_GetAngle(xf.q);
    const float c = std::cos(ang), s = std::sin(ang);

    b2Vec2 forward = { -s,  c };   // girar (0,1) Y
    b2Vec2 right   = {  c,  s };   // girar (1,0) X

    const b2Vec2 v  = b2Body_GetLinearVelocity(body);
    const float  w  = b2Body_GetAngularVelocity(body);

    float v_long = v.x * forward.x + v.y * forward.y;  // proyección v·forward
    float v_lat  = v.x * right.x   + v.y * right.y;    // proyección v·right

    auto m = b2Body_GetMass(body);
    auto aMax = 10; //cada seg la velocidad aumneta como mucho 10 "velocity per second"
    float engineForce = m * aMax;

    float Fengine  = throttle * engineForce;
    b2Vec2 F = { forward.x * Fengine, forward.y * Fengine };
    b2Body_ApplyForceToCenter(car.body, F, true);
}
*/

