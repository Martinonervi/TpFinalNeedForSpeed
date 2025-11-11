#ifndef TPFINALNEEDFORSPEED_CHECKPOINT_H
#define TPFINALNEEDFORSPEED_CHECKPOINT_H

#include <box2d/box2d.h>
#include "entity.h"


enum class CheckpointKind {
    Start,
    Normal,
    Finish
};

class Checkpoint : public Entity {
public:
    Checkpoint(WorldManager& world,
                           int checkpointId, CheckpointKind kind,
                           float x1, float y1,
                           float x2, float y2);

    CheckpointKind getKind() const { return kind; }

    float getX1() const { return x1; }
    float getY1() const { return y1; }
    float getX2() const { return x2; }
    float getY2() const { return y2; }
    ID getId() const { return this->checkpointId; }


private:
    ID checkpointId;       // orden del recorrido
    CheckpointKind kind;
    float x1, y1, x2, y2;
};

#endif






