#ifndef TPFINALNEEDFORSPEED_BUILDING_H
#define TPFINALNEEDFORSPEED_BUILDING_H

#include "entity.h"
#include <box2d/box2d.h>


class Building : public Entity {
public:
    Building(WorldManager& world,
             float x, float y,
             float w, float h,
             float angle)
            : Entity(EntityType::Building, b2_nullBodyId, 0)
              , x(x), y(y)
              , w(w), h(h)
              , angle(angle)
    {
        uint32_t physId = world.createBuilding(x, y, w, h, angle);
        this->setPhysicsId(physId);

        b2BodyId body = world.getBody(physId);
        this->setBody(body);


        auto* ud = new PhysicsUserData{
                .type = PhysicsType::Building,
                .id = 0
        };
        b2Body_SetUserData(this->body, ud);

        // edificios no necesitan hitEvents, con el auto ya alcanza creo
        b2Body_EnableContactEvents(this->body, true);
    }


    float getX() const { return x; }
    float getY() const { return y; }
    float getW() const { return w; }
    float getH() const { return h; }
    float getAngle() const { return angle; }

    void setBody(b2BodyId b) { body = b; }

private:
    float x, y;
    float w, h;
    float angle;
};

#endif




