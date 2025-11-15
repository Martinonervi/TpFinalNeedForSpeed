#ifndef TPFINALNEEDFORSPEED_ENTITY_H
#define TPFINALNEEDFORSPEED_ENTITY_H
#include <box2d/box2d.h>
#include "../../common_src/constants.h"

enum class EntityType {
    Car,
    Checkpoint,
    Building,
};

class WorldManager;

class Entity {
public:
    Entity(EntityType type, b2BodyId body, uint32_t physicsId):
            type(type), body(body), physicsId(physicsId) {}

    virtual ~Entity() = default;
    EntityType getType() const { return type; }

    b2BodyId getBody() const { return body; }
    void setBody(b2BodyId b) { body = b; }

    ID getPhysicsId() const { return physicsId; }
    void setPhysicsId(ID id) { physicsId = id; }

protected:
    EntityType type;
    b2BodyId body;
    ID physicsId;
};

#endif
