#include "world_manager.h"

WorldManager::WorldManager(std::queue<WorldEvent>& worldEvents): worldContactHandler(worldEvents) {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 0.0f};
    this->world = b2CreateWorld(&worldDef);
    worldContactHandler.init(world);
}

void WorldManager::step(float dt, int subSteps) {
    b2World_Step(this->world, dt, subSteps);
    worldContactHandler.checkContactEvents();
}

b2BodyId WorldManager::getBody(const EntityId id) const {
    auto it = physics.find(id);
    if (it == physics.end()) return {0};
    return it->second.body;
}

static inline void destroyBodyAndUserData(b2BodyId body) {
    if (!body.index1) return;
    void* raw = b2Body_GetUserData(body);
    if (raw) {
        b2Body_SetUserData(body, nullptr);
        auto* ud = static_cast<PhysicsUserData*>(raw);
        delete ud;
    }
    b2DestroyBody(body);
}

void WorldManager::destroyEntity(const EntityId id) {
    auto it = physics.find(id);
    if (it == physics.end()) return;
    destroyBodyAndUserData(it->second.body);
    physics.erase(it);
}

WorldManager::~WorldManager() {
    for (auto& [eid, pe] : physics) {
        destroyBodyAndUserData(pe.body);
    }
    physics.clear();
    if (world.index1) {
        b2DestroyWorld(this->world);
        this->world = {0};
    }
}


EntityId WorldManager::createCarBody(b2Vec2 pos, float angleRad) {

    // defino body
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = pos;
    bd.rotation = b2MakeRot(angleRad);
    bd.linearDamping = 0.6f; // [0.8, 1.2]
    bd.angularDamping = 3.f; //reduce velocidad anguar (mientras +alto)

    b2BodyId body = b2CreateBody(world, &bd);

    //creo shape
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = 1.0f; //Masa = densidad * área de la figura
    // aceleracion = F / masa
    sd.material.friction = 1.1f; // afecta mas en vel lateral cuando doblas o derrapas
    sd.material.restitution = 0.08f;
    b2Polygon box = b2MakeBox(1.0f, 2.0f); //rectangulo 2x4

    sd.isSensor = false; // no sensor
    sd.enableSensorEvents = true; //asi los sensores lo ven

    b2CreatePolygonShape(body, &sd, &box);

    b2Body_EnableContactEvents(body, true);
    b2Body_EnableHitEvents(body, true);

    EntityId eid = nextId++;
    physics[eid] = PhysicsEntity{ body, PhysicsEntity::Kind::Car };
    return eid;
}

EntityId WorldManager::createCheckpointSensor(float cx, float cy,
                                              float w, float h,
                                              float angleRad) {
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = (b2Vec2){ cx, cy };
    bd.rotation = b2MakeRot(angleRad);
    b2BodyId body = b2CreateBody(world, &bd);

    float halfW = std::max(w * 0.5f, 0.05f);  // mínimo por las dudas
    float halfH = std::max(h * 0.5f, 0.05f);

    b2Polygon box = b2MakeBox(halfW, halfH);

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.isSensor = true;            // no genera respuesta física
    sd.enableSensorEvents = true;  // habilita eventos de sensor
    b2CreatePolygonShape(body, &sd, &box);

    EntityId eid = nextId++;
    physics[eid] = PhysicsEntity{ body, PhysicsEntity::Kind::Checkpoint };
    return eid;
}


EntityId WorldManager::createBuilding(float x, float y,
                                      float w, float h,
                                      float angleRad)
{
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = (b2Vec2){ x, y }; // centro del edificio
    bd.rotation = b2MakeRot(angleRad);

    b2BodyId body = b2CreateBody(this->world, &bd);

    b2Polygon box = b2MakeBox(w * 0.5f, h * 0.5f);

    b2ShapeDef sd = b2DefaultShapeDef();
    b2CreatePolygonShape(body, &sd, &box);

    // edificios no necesitan hitEvents, con el auto ya alcanza creo
    b2Body_EnableContactEvents(body, true);

    EntityId eid = nextId++;
    physics[eid] = PhysicsEntity{ body, PhysicsEntity::Kind::Building };
    return eid;

}


void WorldManager::resetCheckpoints(std::unordered_map<ID, Checkpoint>& checkpoints) {
    for (auto& [id, cp] : checkpoints) {
        destroyEntity(cp.getPhysicsId());
    }
}