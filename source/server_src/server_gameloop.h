#include <list>
#include <map>

#include "server_client_registry.h"
#include "server_types.h"

#include <box2d/box2d.h>

struct Car {
    b2BodyId body{0};

    void create(b2WorldId world, b2Vec2 pos, float angleRad) {
        b2BodyDef bd = b2DefaultBodyDef();
        bd.type = b2_dynamicBody;
        bd.position = pos;
        bd.rotation = b2MakeRot(angleRad);
        bd.linearDamping = 2.0f;
        bd.angularDamping = 3.0f;
        body = b2CreateBody(world, &bd);

        b2Polygon box = b2MakeBox(1.0f, 2.0f);
        b2ShapeDef sd = b2DefaultShapeDef();
        sd.density = 1.0f;
        // sd.friction = 0.0f;
        // sd.restitution = 0.0f;
        b2CreatePolygonShape(body, &sd, &box);
    }
};

class GameLoop: public Thread {

public:
    GameLoop(gameLoopQueue& queue, ClientsRegistry& registry);

    void stop() override;
    virtual ~GameLoop();

protected:
    void run() override;  // lógica disparadora y el gameloop en si

private:
    void processTrun();  // lógica de negocios (procesamiento)

    std::list<Cmd> emptyQueue();  // vacía cola para comenzar a procesar


    void movementHandler(SrvMsg& msg, Cmd& cmd);

    b2WorldId world;
    std::map<ID, Car> cars;

    std::map<ID, int> nitros;
    gameLoopQueue& queue;  // cola que consume el gameloop
    ClientsRegistry& registry;
    Printer printer;
};


