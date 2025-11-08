#include "world_contact_handler.h"
#include "iostream"

void WorldContactHandler::checkContactEvents() {
    b2ContactEvents contacts = b2World_GetContactEvents(this->world);

    //colisinoes
    //std::cout << "aca en checkContactEvents chequeando colisiones\n";
    /*
    std::cout << "begin: " << contacts.beginCount
              << " end: " << contacts.endCount
              << " hit: " << contacts.hitCount << "\n";
*/
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
    // obtengo bodies desde las shapes
    b2BodyId bodyA = b2Shape_GetBody(ev.shapeIdA);
    b2BodyId bodyB = b2Shape_GetBody(ev.shapeIdB);

    // obtengo user data (PhysicsType, clientId, this)
    auto* udA = static_cast<PhysicsUserData*>(b2Body_GetUserData(bodyA));
    auto* udB = static_cast<PhysicsUserData*>(b2Body_GetUserData(bodyB));
    if (!udA || !udB) return;

    //caclulo que voy a tener que encolar eventos al gameloop o al cliente
    if (udA->type == PhysicsType::Car && udB->type == PhysicsType::Car) {
        std::cout << "BEGIN: car " << udA->clientId << " tocó a car " << udB->clientId << "\n";
        return;
    }

    if (udA->type == PhysicsType::Car && udB->type == PhysicsType::StaticObstacle) {
        std::cout << "BEGIN: car " << udA->clientId << " tocó obstáculo\n";
        return;
    }

    if (udB->type == PhysicsType::Car && udA->type == PhysicsType::StaticObstacle) {
        std::cout << "BEGIN: car " << udB->clientId << " tocó obstáculo\n";
        return;
    }
}

void WorldContactHandler::handleEndContact(b2ContactEndTouchEvent ev){}
void WorldContactHandler::handleHitContact(b2ContactHitEvent ev){}