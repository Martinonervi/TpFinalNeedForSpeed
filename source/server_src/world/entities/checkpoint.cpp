#include "checkpoint.h"

#include "../world_manager.h"

Checkpoint::Checkpoint(WorldManager& world, ID checkpointId, CheckpointKind kind,
                       float cx, float cy, float w, float h, float angleRad)
        : Entity(EntityType::Checkpoint, b2_nullBodyId, 0)
          ,checkpointId(checkpointId)
          ,kind(kind)
          ,x(cx)
          ,y(cy)
          ,w(w)
          ,h(h)
          ,angle(angleRad)
{
    ID physId = world.createCheckpointSensor(cx, cy, w, h, angleRad);
    this->setPhysicsId(physId);

    b2BodyId body = world.getBody(physId);
    this->setBody(body);

    auto* ud = new PhysicsUserData{
        .type = PhysicsType::Checkpoint,
        .id = checkpointId
};
    b2Body_SetUserData(this->body, ud);
}
