#include "building.h"

#include "../world_manager.h"

Building::Building(WorldManager& world,
            float x, float y,
            float w, float h,
            float angle)
           : Entity(EntityType::Building, b2_nullBodyId, 0)
             , x(x), y(y)
             , w(w), h(h)
             , angle(angle)
{
    uint32_t physId = world.createBuilding(x, y, w, h, angle);
    this->setPhysicsId(physId);

    b2BodyId body = world.getBody(physId);
    this->setBody(body);

    auto* ud = new PhysicsUserData{
        .type = PhysicsType::Building,
        .id = 0
};
    b2Body_SetUserData(this->body, ud);
}