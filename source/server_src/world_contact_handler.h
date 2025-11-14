#ifndef TPFINALNEEDFORSPEED_WORLD_CONTACT_LISTENER_H
#define TPFINALNEEDFORSPEED_WORLD_CONTACT_LISTENER_H
#include "box2d/box2d.h"
#include "../common_src/constants.h"
#include "checkpoint.h"
#include "car.h"

enum class WorldEventType {
    CarHitCheckpoint,
    CarHitBuilding,
    CarHitCar,
};

struct WorldEvent {
    WorldEventType type;
    ID carId;
    ID otherCarId;
    ID checkpointId;
    float nx;
    float ny;
};

enum class PhysicsType {
    Car,
    Building,
    Checkpoint,
};


struct PhysicsUserData {
    PhysicsType type;
    ID id;
};

class WorldContactHandler {
public:
    explicit WorldContactHandler(std::queue<WorldEvent>& we): worldEvents(we) {}
    void init(b2WorldId w) { world = w;}
    void checkContactEvents();

private:
    b2WorldId world{};
    std::queue<WorldEvent>& worldEvents;

    void handleHitContact(b2ContactHitEvent ev);
    void handleSensorBegin(const b2SensorBeginTouchEvent& ev);

};

#endif