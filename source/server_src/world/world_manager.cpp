#include "world_manager.h"

WorldManager::WorldManager(std::queue<WorldEvent>& worldEvents): worldContactHandler(worldEvents) {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 0.0f};
    this->world = b2CreateWorld(&worldDef);
    mapLimits();
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
        b2Body_SetUserData(body, nullptr);   // por seguridad para Box2D
        auto* ud = static_cast<PhysicsUserData*>(raw);
        delete ud;                            // ← este era el leak (16 bytes c/u)
    }
    b2DestroyBody(body);
}
/*
void WorldManager::destroyEntity(const EntityId id) {
    auto it = physics.find(id);
    if (it == physics.end()) return;
    b2DestroyBody(it->second.body);
    physics.erase(it);
}
*/
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

void WorldManager::mapLimits() {
    const float MAP_W_PIXELES = 4640.0f;
    const float MAP_H_PIXELES = 4672.0f;

    const float MAP_W = MAP_W_PIXELES * PIXEL_TO_METER;
    const float MAP_H = MAP_H_PIXELES * PIXEL_TO_METER;

    const float T = 1.0f; // grosor de la pared

    // pared de arriba
    {
        b2BodyDef bd = b2DefaultBodyDef();
        bd.type = b2_staticBody;
        bd.position = (b2Vec2){ MAP_W * 0.5f, -T };
        b2BodyId body = b2CreateBody(this->world, &bd);

        b2Polygon box = b2MakeBox(MAP_W * 0.5f, T);
        b2ShapeDef sd = b2DefaultShapeDef();
        b2CreatePolygonShape(body, &sd, &box);

        EntityId eid = nextId++;
        physics[eid] = PhysicsEntity{ body, PhysicsEntity::Kind::Building };

        auto* ud = new PhysicsUserData{
            .type = PhysicsType::Building,
            .id   = eid
        };
        b2Body_SetUserData(body, ud);
        b2Body_EnableContactEvents(body, true);
        b2Body_EnableHitEvents(body, true);
    }

    // pared de abajo
    {
        b2BodyDef bd = b2DefaultBodyDef();
        bd.type = b2_staticBody;
        bd.position = (b2Vec2){ MAP_W * 0.5f, MAP_H + T };
        b2BodyId body = b2CreateBody(this->world, &bd);

        b2Polygon box = b2MakeBox(MAP_W * 0.5f, T);
        b2ShapeDef sd = b2DefaultShapeDef();
        b2CreatePolygonShape(body, &sd, &box);

        EntityId eid = nextId++;
        physics[eid] = PhysicsEntity{ body, PhysicsEntity::Kind::Building };

        auto* ud = new PhysicsUserData{
            .type = PhysicsType::Building,
            .id   = eid
        };
        b2Body_SetUserData(body, ud);
        b2Body_EnableContactEvents(body, true);
        b2Body_EnableHitEvents(body, true);
    }

    // pared izquierda
    {
        b2BodyDef bd = b2DefaultBodyDef();
        bd.type = b2_staticBody;
        bd.position = (b2Vec2){ -T, MAP_H * 0.5f };
        b2BodyId body = b2CreateBody(this->world, &bd);

        b2Polygon box = b2MakeBox(T, MAP_H * 0.5f);
        b2ShapeDef sd = b2DefaultShapeDef();
        b2CreatePolygonShape(body, &sd, &box);

        EntityId eid = nextId++;
        physics[eid] = PhysicsEntity{ body, PhysicsEntity::Kind::Building };

        auto* ud = new PhysicsUserData{
            .type = PhysicsType::Building,
            .id   = eid
        };
        b2Body_SetUserData(body, ud);
        b2Body_EnableContactEvents(body, true);
        b2Body_EnableHitEvents(body, true);
    }

    // pared derecha
    {
        b2BodyDef bd = b2DefaultBodyDef();
        bd.type = b2_staticBody;
        bd.position = (b2Vec2){ MAP_W + T, MAP_H * 0.5f };
        b2BodyId body = b2CreateBody(this->world, &bd);

        b2Polygon box = b2MakeBox(T, MAP_H * 0.5f);
        b2ShapeDef sd = b2DefaultShapeDef();
        b2CreatePolygonShape(body, &sd, &box);

        EntityId eid = nextId++;
        physics[eid] = PhysicsEntity{ body, PhysicsEntity::Kind::Building };

        auto* ud = new PhysicsUserData{
            .type = PhysicsType::Building,
            .id   = eid
        };
        b2Body_SetUserData(body, ud);
        b2Body_EnableContactEvents(body, true);
        b2Body_EnableHitEvents(body, true);
    }
}



EntityId WorldManager::createCarBody(b2Vec2 pos, float angleRad) {

    // defino body
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = pos;
    bd.rotation = b2MakeRot(angleRad);
    bd.linearDamping = 0.25f;
    bd.angularDamping = 3.f; //reduce velocidad anguar (mientras +alto)

    // creo body
    b2BodyId body = b2CreateBody(world, &bd);

    //creo shape
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = 1.0f; //Masa = densidad * área de la figura
    // aceleracion = F / masa
    sd.material.friction = 0.9f; // afecta mas en vel lateral cuando doblas o derrapas
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


EntityId WorldManager::createCheckpointSensor(float x1, float y1,
                                              float x2, float y2) {
    float mx = (x1 + x2) * 0.5f;
    float my = (y1 + y2) * 0.5f;

    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = std::sqrt(dx*dx + dy*dy);
    float angle  = std::atan2(dy, dx);

    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = (b2Vec2){ mx, my };
    bd.rotation = b2MakeRot(angle);
    b2BodyId body = b2CreateBody(world, &bd);

    float halfLen = length * 0.5f;
    float halfThickness = 1.0f;
    b2Polygon box = b2MakeBox(halfLen, halfThickness);

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.isSensor = true; //no genera resp fisica en colision
    sd.enableSensorEvents = true; //habilita el sensor contact
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

    // media base y media altura
    b2Polygon box = b2MakeBox(w * 0.5f, h * 0.5f);

    b2ShapeDef sd = b2DefaultShapeDef();
    b2CreatePolygonShape(body, &sd, &box);

    // edificios no necesitan hitEvents, con el auto ya alcanza creo
    b2Body_EnableContactEvents(body, true);

    EntityId eid = nextId++;
    physics[eid] = PhysicsEntity{ body, PhysicsEntity::Kind::Building };
    return eid;

}
