#include "world_manager.h"

WorldManager::WorldManager() {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 0.0f};  // sin gravedad
    this->world = b2CreateWorld(&worldDef);
    mapLimits();
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

b2BodyId WorldManager::getBody(const EntityId id) const {
    auto it = physics.find(id);
    if (it == physics.end()) return {0};
    return it->second.body;
}

void WorldManager::destroyEntity(const EntityId id) {
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

void WorldManager::mapLimits() {
        const float MAP_W = 696.0f;   // ancho
        const float MAP_H = 700.8f;   // alto

        const float T = 1.0f; // grosor de la pared

        // user data ??

        // pared de arriba
        {
            b2BodyDef bd = b2DefaultBodyDef();
            bd.type = b2_staticBody;
            // centro de la pared de arriba
            bd.position = (b2Vec2){MAP_W * 0.5f, -T}; //centro , (mitad X, -1 Y)
            b2BodyId body = b2CreateBody(this->world, &bd);

            b2Polygon box = b2MakeBox(MAP_W * 0.5f, T); //2*MAP_W
            b2ShapeDef sd = b2DefaultShapeDef();
            b2CreatePolygonShape(body, &sd, &box);
        }

        // pared de abajo
        {
            b2BodyDef bd = b2DefaultBodyDef();
            bd.type = b2_staticBody;
            bd.position = (b2Vec2){MAP_W * 0.5f, MAP_H + T};
            b2BodyId body = b2CreateBody(this->world, &bd);

            b2Polygon box = b2MakeBox(MAP_W * 0.5f, T);
            b2ShapeDef sd = b2DefaultShapeDef();
            b2CreatePolygonShape(body, &sd, &box);
        }

        // pared izquierda
        {
            b2BodyDef bd = b2DefaultBodyDef();
            bd.type = b2_staticBody;
            bd.position = (b2Vec2){-T, MAP_H * 0.5f};
            b2BodyId body = b2CreateBody(this->world, &bd);

            b2Polygon box = b2MakeBox(T, MAP_H * 0.5f);
            b2ShapeDef sd = b2DefaultShapeDef();
            b2CreatePolygonShape(body, &sd, &box);
        }

        // pared derecha
        {
            b2BodyDef bd = b2DefaultBodyDef();
            bd.type = b2_staticBody;
            bd.position = (b2Vec2){MAP_W + T, MAP_H * 0.5f};
            b2BodyId body = b2CreateBody(this->world, &bd);

            b2Polygon box = b2MakeBox(T, MAP_H * 0.5f);
            b2ShapeDef sd = b2DefaultShapeDef();
            b2CreatePolygonShape(body, &sd, &box);
        }
}

