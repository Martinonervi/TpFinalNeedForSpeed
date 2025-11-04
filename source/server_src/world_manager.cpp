#include "world_manager.h"

WorldManager::WorldManager() {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 0.0f};  // sin gravedad
    this->world = b2CreateWorld(&worldDef);
    worldContactHandler.init(world);
}

void WorldManager::step(float dt, int subSteps) {
    b2World_Step(this->world, dt, subSteps);

    worldContactHandler.checkContactEvents();
}

EntityId WorldManager::createCarBody(b2Vec2 pos, float angleRad) {

    // defino body
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = pos;
    bd.rotation = b2MakeRot(angleRad);
    bd.linearDamping = 0.2f;
    bd.angularDamping = 4.f; //reduce velocidad anguar (mientras +alto)

    // creo body
    b2BodyId body = b2CreateBody(world, &bd);

    //creo shape
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = 1.0f; //Masa = densidad * área de la figura
    // aceleracion = F / masa
    sd.material.friction = 0.9f; // afecta mas en vel lateral cuando doblas o derrapas
    sd.material.restitution = 0.08f;
    b2Polygon box = b2MakeBox(1.0f, 2.0f); //rectangulo 2x4

    b2CreatePolygonShape(body, &sd, &box);


    EntityId eid = nextId++;
    physics[eid] = PhysicsEntity{ body, PhysicsEntity::Kind::Car };
    return eid;
}

b2BodyId WorldManager::getBody(EntityId id) const {
    auto it = physics.find(id);
    if (it == physics.end()) return {0};
    return it->second.body;
}

void WorldManager::destroyEntity(EntityId id) {
    auto it = physics.find(id);
    if (it == physics.end()) return;
    b2DestroyBody(it->second.body);
    physics.erase(it);
}

WorldManager::~WorldManager() {
    if (world.index1) {
        b2DestroyWorld(this->world);
        this->world = {0};
    }
}

