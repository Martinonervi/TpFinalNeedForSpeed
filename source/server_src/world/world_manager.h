#ifndef TPFINALNEEDFORSPEED_WORLD_MANAGER_H
#define TPFINALNEEDFORSPEED_WORLD_MANAGER_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <box2d/box2d.h>
#include "../../common_src/constants.h"
#include "world_contact_handler.h"


using EntityId = uint32_t;

struct PhysicsEntity {
    b2BodyId body;
    enum class Kind {
        Car,
        Building,
        Checkpoint,
    } kind;
};

class WorldManager {
public:
    explicit WorldManager(std::queue<WorldEvent>& worldEvents);
    ~WorldManager();
    void step(float dt, int subSteps);

    EntityId createCarBody(b2Vec2 pos, float angleRad);
    EntityId createBuilding(float x, float y, float w, float h, float angleRad);
    EntityId createCheckpointSensor(float cx, float cy,
                                              float w, float h,
                                              float angleRad);

    b2BodyId getBody(EntityId id) const;

    void destroyEntity(EntityId id);
    void resetCheckpoints(std::unordered_map<ID, Checkpoint>& checkpoints);


private:
    b2WorldId world{};
    EntityId nextId = 1;
    std::unordered_map<EntityId, PhysicsEntity> physics;
    WorldContactHandler worldContactHandler;
};


#endif
