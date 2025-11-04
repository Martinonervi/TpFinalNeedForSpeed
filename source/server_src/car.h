#pragma once
#include <box2d/box2d.h>
#include "../common_src/constants.h"
#include "../common_src/move_Info.h"
#include "../common_src/player_state.h"

class Car {
public:
    Car(ID id, b2BodyId body);
    void applyControlsToBody(const MoveMsg& in, float dt);
    PlayerState snapshotState();
private:
    ID id;
    b2BodyId body;

};

