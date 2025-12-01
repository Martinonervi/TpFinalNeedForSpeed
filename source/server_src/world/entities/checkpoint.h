#ifndef TPFINALNEEDFORSPEED_CHECKPOINT_H
#define TPFINALNEEDFORSPEED_CHECKPOINT_H
#include "entity.h"

enum class CheckpointKind {
    Start,
    Normal,
    Finish
};

class WorldManager;

class Checkpoint : public Entity {
public:
    Checkpoint(WorldManager& world,
               ID checkpointId, CheckpointKind kind,
               float cx, float cy,
               float w, float h,
               float angleRad);

    CheckpointKind getKind() const { return kind; }
    ID getId() const { return checkpointId; }


    float getX() const { return x; }
    float getY() const { return y; }
    float getW() const { return w; }
    float getH() const { return h; }
    float getAngle() const { return angle; }

private:
    ID checkpointId;       // orden del recorrido
    CheckpointKind kind;

    float x, y;
    float w, h;
    float angle;
};

#endif
