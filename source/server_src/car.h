#pragma once
#include <box2d/box2d.h>
#include "../common_src/constants.h"
#include "../common_src/move_Info.h"
#include "../common_src/player_state.h"

class Car {
public:
    Car(ID id, b2WorldId world, b2Vec2 pos, float angleRad);
    PlayerState snapshotState();
    void applyControlsToBody(const MoveMsg& in, float dt);

private:
    ID id;
    b2BodyId body;
    //float health{100.f};

};

