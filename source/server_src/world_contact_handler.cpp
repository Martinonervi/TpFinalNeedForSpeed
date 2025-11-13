#include "world_contact_handler.h"
#include "iostream"

void WorldContactHandler::checkContactEvents() {
    b2ContactEvents contacts = b2World_GetContactEvents(this->world);

    //std::cout << "aca en checkContactEvents chequeando colisiones\n";

    /*std::cout << "begin: " << contacts.beginCount
              << " end: " << contacts.endCount
              << " hit: " << contacts.hitCount << "\n";*/

    for (int i = 0; i < contacts.beginCount; ++i) {
        const b2ContactBeginTouchEvent& ev = contacts.beginEvents[i];
        handleBeginContact(ev);
    }

    for (int i = 0; i < contacts.endCount; ++i) {
        const b2ContactEndTouchEvent& ev = contacts.endEvents[i];
        handleEndContact(ev);
    }

    for (int i = 0; i < contacts.hitCount; ++i) {
        const b2ContactHitEvent& ev = contacts.hitEvents[i];
        handleHitContact(ev);
    }
}

void WorldContactHandler::handleBeginContact(const b2ContactBeginTouchEvent& ev) {
    b2BodyId bodyA = b2Shape_GetBody(ev.shapeIdA);
    b2BodyId bodyB = b2Shape_GetBody(ev.shapeIdB);

    PhysicsUserData* udA = static_cast<PhysicsUserData*>(b2Body_GetUserData(bodyA));
    PhysicsUserData* udB = static_cast<PhysicsUserData*>(b2Body_GetUserData(bodyB));
    if (!udA || !udB) return;

    PhysicsUserData* car = nullptr;
    PhysicsUserData* cp = nullptr;

    if (udA->type == PhysicsType::Car && udB->type == PhysicsType::Checkpoint) {
        car = udA;
        cp = udB;
        std::cout << "auto pasando por checkpoint\n";

    } else if (udB->type == PhysicsType::Car && udA->type == PhysicsType::Checkpoint) {
        car = udB;
        cp = udA;
        std::cout << "auto pasando por checkpoint\n";
    } else {
        return;
    }

    WorldEvent event{
            .type = WorldEventType::CarHitCheckpoint,
            .carId = car->id,
            .checkpointId = cp->id,
    };
    worldEvents.push(event);
}



void WorldContactHandler::handleEndContact(b2ContactEndTouchEvent ev){}

void WorldContactHandler::handleHitContact(b2ContactHitEvent ev) {
    std::cout << "entre a handler hit contact\n";
    b2BodyId bodyA = b2Shape_GetBody(ev.shapeIdA);
    b2BodyId bodyB = b2Shape_GetBody(ev.shapeIdB);

    auto* udA = static_cast<PhysicsUserData*>(b2Body_GetUserData(bodyA));
    auto* udB = static_cast<PhysicsUserData*>(b2Body_GetUserData(bodyB));
    if (!udA || !udB) return;

    if (udA->type == PhysicsType::Car && udB->type == PhysicsType::Car) {
        WorldEvent evOut{
                .type = WorldEventType::CarHitCar,
                .carId = udA->id,
                .otherCarId = udB->id,
                .checkpointId = 0,
                .nx = ev.normal.x,
                .ny = ev.normal.y,
        };
        worldEvents.push(evOut);
        std::cout << "auto choco con auto \n";
        return;
    }

    PhysicsUserData* car = nullptr;
    PhysicsUserData* building = nullptr;

    if (udA->type == PhysicsType::Car && udB->type == PhysicsType::Building) {
        car = udA;
        //building = udB;
    } else if (udB->type == PhysicsType::Car && udA->type == PhysicsType::Building) {
        car = udB;
        //building = udA;
    } else {
        return;
    }

    WorldEvent evOut{
            .type = WorldEventType::CarHitBuilding,
            .carId = car->id,
            .checkpointId = 0,
            .nx = ev.normal.x,
            .ny = ev.normal.y,
    };
    worldEvents.push(evOut);
    std::cout << "auto choco con edificio\n";
    return;



}







