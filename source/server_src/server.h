#include "server_acceptor.h"
#include "server_gameloop.h"
#include "server_types.h"


class Server {
public:
    explicit Server(const char* service);

    int Main();  // disparador de la lógica del servidor
private:
    // frena al acceptor y al gameloop
    void stop_workers();

    // joinea al acceptor y al gameloop
    void join_workers();

    ClientsRegistry registry;
    serv_types::gameLoopQueue cmd_queue;
    GameLoop gameloop;
    Socket acc_sock;
    Acceptor acceptor;
};
