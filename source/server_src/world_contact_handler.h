#ifndef TPFINALNEEDFORSPEED_WORLD_CONTACT_LISTENER_H
#define TPFINALNEEDFORSPEED_WORLD_CONTACT_LISTENER_H
#include "box2d/box2d.h"
#include "../common_src/constants.h"
enum class PhysicsType {
    Car,
    StaticObstacle,
};

struct PhysicsUserData {
    PhysicsType type;
    ID clientId;   // cual de los dos?
    void* owner;
};

class WorldContactHandler {
public:
    WorldContactHandler() = default;
    void init(b2WorldId w) { world = w; }
    void checkContactEvents();

private:
    b2WorldId world;

    void handleBeginContact(const b2ContactBeginTouchEvent& ev);
    void handleEndContact(b2ContactEndTouchEvent ev);
    void handleHitContact(b2ContactHitEvent ev);


};




#endif