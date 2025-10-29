#include <list>
#include <map>

#include "server_client_registry.h"
#include "server_types.h"

#include <box2d/box2d.h>

class GameLoop: public Thread {

public:
    GameLoop(serv_types::gameLoopQueue& queue, ClientsRegistry& registry);

    void stop() override;
    virtual ~GameLoop();

protected:
    void run() override;  // lógica disparadora y el gameloop en si

private:
    void processTrun();  // lógica de negocios (procesamiento)

    std::list<Cmd> emptyQueue();  // vacía cola para comenzar a procesar


    void movementHandler(SrvMsg& msg, Cmd& cmd);

    std::map<serv_types::ID, int> nitros;
    serv_types::gameLoopQueue& queue;  // cola que consume el gameloop
    ClientsRegistry& registry;
    Printer printer;
};
