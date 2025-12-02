#include "client.h"

#include <QApplication>

#include "../common_src/cli_msg/requestgame.h"
#include "../common_src/srv_msg/joingame.h"
#include "lobby/lobbywindow.h"
#include "postgame/postgamewindow.h"

#include "client_window.h"

Client::Client(const char* host, const char* service)
    : peer(host, service), protocol(peer),
receiver(protocol, receiverQueue), sender(protocol, senderQueue)
{}

void Client::run() {

    if (lobbyState()) { return;}

    sender.start();
    receiver.start();

    ClientWindow client_window(
        1200,
        800,
        "Need For Speed",
        receiverQueue,
        senderQueue
    );

    auto [quit, playerStatsPtr] = client_window.run();

    PlayerStats stats_hardcoded(3, 2.66);
    if (quit) {
        PlayerStats stats(playerStatsPtr->getRacePosition(), playerStatsPtr->getTimeSecToComplete());
        postGame(stats);
    }
    stop();
    join();
}

bool Client::lobbyState() {
    int argc = 0;
    char** argv = nullptr;
    QApplication app(argc, argv);
    bool was_closed = false;

    LobbyWindow window(protocol, was_closed);

    window.show();

    app.exec();   // event loop de Qt

    return was_closed;
}

void Client::postGame(PlayerStats& player_stats) {
    int argc = 0;
    char** argv = nullptr;
    QApplication app(argc, argv);

    PostGameWindow win(player_stats);
    win.show();

    app.exec();  // loop de eventos de la pantalla de estadísticas
}

void Client::stop(){
    sender.stop();
    receiver.stop();

    senderQueue.close();
    receiverQueue.close();
    std::cout << "[Client] Sender y Receiver frenados" <<std::endl;
    close();
}

void Client::join(){
    receiver.join();
    std::cout << "[Client] Receiver joineado" <<std::endl;
    sender.join();
    std::cout << "[Client] Sender y Receiver joineados" <<std::endl;
}
void Client::close() {
    try {
        peer.shutdown(2);
        peer.close();
    }catch (...){}
}

