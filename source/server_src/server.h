#include "server_acceptor.h"
#include "server_gameloop.h"
#include "server_types.h"
#include "server_game_manager.h"


class Server {
public:
    explicit Server(const char* service);
    int run();
private:
    // frena al acceptor y al gameloop
    void stop_workers();

    // joinea al acceptor y al gameloop
    void join_workers();

    GameManager game_registry;
    Socket acc_sock;
    Acceptor acceptor;
};
