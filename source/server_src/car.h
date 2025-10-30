#pragma once
#include <box2d/box2d.h>
#include "../common_src/constants.h"

class Car {
public:
    Car(ID id, b2WorldId world, b2Vec2 pos, float angleRad);
    void snapshotState(PlayerStateUpdate& ps);
    void applyControlsToBody(const MoveInfo& in, float dt);

private:
    ID id;
    b2BodyId body;
    //float health{100.f};

};

