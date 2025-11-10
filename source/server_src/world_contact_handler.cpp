#include "world_contact_handler.h"
#include "iostream"

void WorldContactHandler::checkContactEvents() {
    b2ContactEvents contacts = b2World_GetContactEvents(this->world);

    //std::cout << "aca en checkContactEvents chequeando colisiones\n";

    std::cout << "begin: " << contacts.beginCount
              << " end: " << contacts.endCount
              << " hit: " << contacts.hitCount << "\n";

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

    auto* udA = static_cast<PhysicsUserData*>(b2Body_GetUserData(bodyA));
    auto* udB = static_cast<PhysicsUserData*>(b2Body_GetUserData(bodyB));
    if (!udA || !udB) return;

    Car* car = nullptr;
    Checkpoint* cp = nullptr;

    if (udA->type == PhysicsType::Car && udB->type == PhysicsType::Checkpoint) {
        car = static_cast<Car*>(udA->owner);
        cp  = static_cast<Checkpoint*>(udB->owner);
        if (!car || !cp) return;

    } else if (udB->type == PhysicsType::Car && udA->type == PhysicsType::Checkpoint) {
        car = static_cast<Car*>(udB->owner);
        cp  = static_cast<Checkpoint*>(udA->owner);
        if (!car || !cp) return;
    } else {
        return;
    }

    WorldEvent event{
            .type = WorldEventType::CarHitCheckpoint,
            .carId = car->getClientId(),
            .checkpointId = cp->getId(),
    };
    worldEvents.push(event);
}



void WorldContactHandler::handleEndContact(b2ContactEndTouchEvent ev){}


void WorldContactHandler::handleHitContact(b2ContactHitEvent ev){
    b2BodyId bodyA = b2Shape_GetBody(ev.shapeIdA);
    b2BodyId bodyB = b2Shape_GetBody(ev.shapeIdB);

    auto* udA = static_cast<PhysicsUserData*>(b2Body_GetUserData(bodyA));
    auto* udB = static_cast<PhysicsUserData*>(b2Body_GetUserData(bodyB));
    if (!udA || !udB) return;

    Car* car = nullptr;

    if (udA->type == PhysicsType::Car && udB->type == PhysicsType::Building) {
        car = static_cast<Car*>(udA->owner);
        if (!car) return;
    } else if (udA->type == PhysicsType::Building && udB->type == PhysicsType::Car) {
        car = static_cast<Car*>(udB->owner);
        if (!car) return;
    } else {
        return;
    }

    //idea: si el auto se mantiene en contacto con el edificio va a recibir muchos hit,
    // eso esta bien? sino habria que poner un contador, mejor hacerlo con el gameloop optimizado.

    WorldEvent event{
            .type = WorldEventType::CarHitBuilding,
            .carId = car->getClientId(),
            .checkpointId = 0,
            .nx = ev.normal.x,  // normal.x
            .ny = ev.normal.y,  // normal.y
    };
    worldEvents.push(event);
    //hitEvent trae info del hit.


}






