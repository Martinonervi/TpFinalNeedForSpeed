#include "world_contact_handler.h"
#include "iostream"

void WorldContactHandler::checkContactEvents() {
    b2ContactEvents contacts = b2World_GetContactEvents(this->world);

    /*
    std::cout << "begin: " << contacts.beginCount
              << " end: " << contacts.endCount
              << " hit: " << contacts.hitCount << "\n";
    */
    // colision autos
    for (int i = 0; i < contacts.hitCount; ++i) {
        const b2ContactHitEvent& ev = contacts.hitEvents[i];
        handleHitContact(ev);
    }

    /* los dejo por si en un futuro los necesito
    for (int i = 0; i < contacts.beginCount; ++i) {
        const b2ContactBeginTouchEvent& ev = contacts.beginEvents[i];
        handleBeginContact(ev);
    }
     for (int i = 0; i < contacts.endCount; ++i) {
        const b2ContactEndTouchEvent& ev = contacts.endEvents[i];
        handleEndContact(ev);
    }
    */

    //checkpoints
    b2SensorEvents sensors = b2World_GetSensorEvents(this->world);
    for (int i = 0; i < sensors.beginCount; ++i) {
        const b2SensorBeginTouchEvent& ev = sensors.beginEvents[i];
        handleSensorBegin(ev);
    }

}

void WorldContactHandler::handleSensorBegin(const b2SensorBeginTouchEvent& ev) {
    b2BodyId sensorBody  = b2Shape_GetBody(ev.sensorShapeId);
    b2BodyId visitorBody = b2Shape_GetBody(ev.visitorShapeId);

    auto* udS = static_cast<PhysicsUserData*>(b2Body_GetUserData(sensorBody));
    auto* udV = static_cast<PhysicsUserData*>(b2Body_GetUserData(visitorBody));

    if (!udS || !udV) return;

    PhysicsUserData* car = nullptr;
    PhysicsUserData* cp  = nullptr;

    if (udS->type == PhysicsType::Checkpoint && udV->type == PhysicsType::Car) {
        cp  = udS;
        car = udV;
    } else if (udV->type == PhysicsType::Checkpoint && udS->type == PhysicsType::Car) {
        cp  = udV;
        car = udS;
    } else {
        return;
    }

    WorldEvent event{
            .type         = WorldEventType::CarHitCheckpoint,
            .carId        = car->id,
            .checkpointId = cp->id,
    };
    worldEvents.push(event);
}

void WorldContactHandler::handleHitContact(b2ContactHitEvent ev) {
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
       // std::cout << "[WorldContactHandler] carA choco con carB (carAId=" << udA->id
         //         << ", carBId=" << udB->id << ")\n";
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
    //std::cout << "[WorldContactHandler] auto choco con edificio\n";
}







