#include "car.h"
#include <algorithm>
#include <cmath>
#include "world_manager.h"

Car::Car(ID id, b2BodyId body)
        : id(id), body(body) {
    auto* ud = new PhysicsUserData{
            PhysicsType::Car,
            id,
            this
    };
    b2Body_SetUserData(this->body, ud);
    b2Body_EnableContactEvents(this->body, true);
    b2Body_EnableHitEvents(this->body, true);
}

static inline float clampf(float x, float a, float b){ return std::max(a, std::min(b, x)); }

void Car::applyControlsToBody(const MoveMsg& in, float dt) {
    // normalizamos
    const float throttle = clampf(in.getAccelerate(), 0.f, 1.f);
    const float brake    = clampf(in.getBrake(),      0.f, 1.f);
    const float steer    = clampf(in.getSteer(),     -1.f, 1.f);

    b2Rot rot   = b2Body_GetRotation(body);
    b2Vec2 right = b2RotateVector(rot, {1.f, 0.f}); //veo donde es 'derecha' en el auto
    b2Vec2 fwd   = b2RotateVector(rot, {0.f, 1.f}); //veo donde es 'adelante' en el auto

    // Velocidad actual
    b2Vec2 v = b2Body_GetLinearVelocity(body);
    float v_long = v.x * fwd.x + v.y * fwd.y; //vector velocidad para adelante
    float v_lat  = v.x * right.x + v.y * right.y; //vector velocidad para la derecha

    // Fuerzas longitudinales (tuneables segun masa diria)
    const float engineForce = 600.f;   // N
    const float brakeForce  = 1200.f;  // N
    const float c_drag      = 12.f;    // N/(m/s) cuanto frena por el piso, frena solo
    const float maxSpeed    = 8.0f;   // m/s  tope de velocidad

    b2Vec2 F = {0.f, 0.f}; // voy a ir sumando las fuerzas

    // Motor
    // std::fabs() -> valor absoluto para floats (por si hago marcha atras)
    if (throttle > 0.f && std::fabs(v_long) < maxSpeed * 1.01f) {
        // a medida que me acerco a la V max la aceleracion es menor
        float scale = 1.f - clampf(std::fabs(v_long)/maxSpeed, 0.f, 1.f);
        F += engineForce * throttle * scale * fwd;
        // enigneForce -> la variable q define cuanto es lo q acelera
        // throttle -> 1 si entro al if
        // scale -> factor para ver si estoy llegando al max
        // fwd -> la velocidad de antes
    }

    // Freno (si vas hacia adelante frena hacia atrás, y viceversa)
    // igual falta implementar la reversa
    if (brake > 0.f) {
        float dir = (v_long >= 0.f) ? -1.f : 1.f;
        F += dir * brakeForce * brake * fwd;
        // dir -> contraria a para donde va el auto
        // brakeForce -> cte
        // fwd -> vector de direccion para adelante
        // brake -> 1
    }

    // Drag longitudinal
    F += (-c_drag * v_long) * fwd;

    b2Body_ApplyForceToCenter(body, F, true);

    // idea: contrarestar el impulso lateral que se general al doblar (anti-derrape lateral)
    float m = b2Body_GetMass(body);
    float slipFactor = 0.75;
    // masa* v_lateral * la direccion lateral * factor -> todo para encontrar el impuslo que
    //  anule el derrape en ese frame. '-m' porq quiero el opuesto, para contrarestar
    b2Vec2 J = (-m * v_lat * slipFactor * right);
    b2Body_ApplyLinearImpulseToCenter(body, J, true);

    // torque, que tanto giras
    // V angular, depende del steer y de la velocidad al doblar
    const float kSteerMin = 800.f;
    const float kSteerMax = 1000.f;
    float speedNorm = clampf(std::fabs(v_long)/maxSpeed, 0.f, 1.f); //que tan cerca de la Vmax
    float steerGain = kSteerMin + (kSteerMax - kSteerMin) * speedNorm;

    float tau = steer * steerGain;
    b2Body_ApplyTorque(body, tau, true);

    if (std::fabs(steer) < 0.01f) {
        float omega   = b2Body_GetAngularVelocity(body);
        float inertia = b2Body_GetRotationalInertia(body);   // ← este es el correcto
        float angularBrake = 10.0f;                           // tunear
        float tauStop = -omega * angularBrake * inertia;
        b2Body_ApplyTorque(body, tauStop, true);

        if (std::fabs(omega) < 0.05f) {
            b2Body_SetAngularVelocity(body, 0.0f);
        }
    }

    /*
    // si no está girando con el volante, freno la rotación
    if (steer == 0.f) {
        float omega = b2Body_GetAngularVelocity(body);
        float angularBrake = 20.0f;          // tuneá esto
        float tauStop = -omega * angularBrake;
        b2Body_ApplyTorque(body, tauStop, true);
    }
*/
    // que en algun momento frene
    const float stopEps = 0.05f;
    if (std::fabs(v_long) < stopEps && throttle == 0.f && brake == 1.f) {
        // Hacé un stop discreto sólo cuando realmente estás casi parado
        b2Vec2 v2 = b2Body_GetLinearVelocity(body);
        // Quitá también residuo lateral pequeño:
        if (std::fabs(v2.x) < 0.02f && std::fabs(v2.y) < 0.02f) {
            b2Body_SetLinearVelocity(body, {0.f, 0.f});
        }
    }
}


PlayerState Car::snapshotState(){
    b2Transform xf = b2Body_GetTransform(body);
    return PlayerState(id, xf.p.x, xf.p.y, b2Rot_GetAngle(xf.q));
}