#ifndef TPFINALNEEDFORSPEED_WORLD_MANAGER_H
#define TPFINALNEEDFORSPEED_WORLD_MANAGER_H

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <string>
#include <box2d/box2d.h>

using EntityId = uint32_t;

struct PhysicsEntity {
    b2BodyId body;
    enum class Kind {
        Car,
        Building,
        // ... después podés agregar Pickup, Obstacle, etc.
    } kind;
};

struct BuildingDesc {
    float x, y;
    float w, h;
    float angle;
};

class WorldManager { //se encarga del manejo del world y de crear los cuerpos
public:
    WorldManager();
    ~WorldManager();
    //void loadMapFromYaml(const std::string& path); // carga todo lo estático del mapa
    void step(float dt, int subSteps);

    EntityId createCarBody(b2Vec2 pos, float angleRad); // crea un auto dinámico y devuelve el id
    b2BodyId getBody(EntityId id) const;

    void destroyEntity(EntityId id);


private:
    b2WorldId world;
    EntityId nextId = 1;

    // todos los que tienen body:
    std::unordered_map<EntityId, PhysicsEntity> physics;

    //void createBuildingFromDesc(const BuildingDesc& d);
};


#endif
