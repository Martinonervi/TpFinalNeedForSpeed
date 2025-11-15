#include "checkpoint.h"
#include "world_manager.h"

Checkpoint::Checkpoint(WorldManager& world, ID checkpointId, CheckpointKind kind,
                       float x1, float y1, float x2, float y2)
        : Entity(EntityType::Checkpoint, b2_nullBodyId, 0)
          , checkpointId(checkpointId)
          , kind(kind)
          , x1(x1), y1(y1)
          , x2(x2), y2(y2)
{

    ID physId = world.createCheckpointSensor(x1, y1, x2, y2);
    this->setPhysicsId(physId);

    b2BodyId body = world.getBody(physId);
    this->setBody(body);

    auto* ud = new PhysicsUserData{
            .type = PhysicsType::Checkpoint,
            .id = checkpointId
    };
    b2Body_SetUserData(this->body, ud);

}


