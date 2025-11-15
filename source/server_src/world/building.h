#ifndef TPFINALNEEDFORSPEED_BUILDING_H
#define TPFINALNEEDFORSPEED_BUILDING_H

#include <box2d/box2d.h>
#include "entity.h"


class Building : public Entity {
public:
    Building(WorldManager& world,
             float x, float y,
             float w, float h,
             float angle);

    float getX() const { return x; }
    float getY() const { return y; }
    float getW() const { return w; }
    float getH() const { return h; }
    float getAngle() const { return angle; }


private:
    float x, y;
    float w, h;
    float angle;
};

#endif




