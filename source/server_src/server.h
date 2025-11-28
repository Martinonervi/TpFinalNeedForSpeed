#include "game_logic/server_gameloop.h"

#include "server_acceptor.h"
#include "server_game_manager.h"
#include "server_types.h"


class Server {
public:

    explicit Server(const char* service);

    // lógica disparadora del servidor
    int run();
private:
    // frena al acceptor
    void stop_workers();

    // joinea al acceptor
    void join_workers();

    GameManager game_registry;
    Socket acc_sock;
    Acceptor acceptor;
};
