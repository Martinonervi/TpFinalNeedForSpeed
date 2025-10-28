#include <list>
#include <map>

#include "server_client_registry.h"
#include "server_types.h"


class GameLoop: public Thread {

public:
    GameLoop(serv_types::gameLoopQueue& queue, ClientsRegistry& registry);

    void stop() override;

protected:
    void run() override;  // lógica disparadora y el gameloop en si

private:
    void processTrun();  // lógica de negocios (procesamiento)

    std::list<constants::Cmd> emptyQueue();  // vacía cola para comenzar a procesar


    std::map<serv_types::ID, int> nitros;
    serv_types::gameLoopQueue& queue;  // cola que consume el gameloop
    ClientsRegistry& registry;
    Printer printer;
};
