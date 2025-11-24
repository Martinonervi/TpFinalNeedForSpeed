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

    /*
    while (std::getline(std::cin, line)) {
        if (line.empty())
            continue;

        int num = std::stoi(line);

        if (num < 0 || num >= CAR_COUNT) {
            std::cerr << "Tipo de auto inválido: " << num << std::endl;
            continue;
        }

        auto type = static_cast<CarType>(num);

        InitPlayer ip("jugador", type);
        CliMsgPtr msg = std::make_shared<InitPlayer>(ip);
        senderQueue.push(msg);
        break;
    }
    */

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
        postGame(stats_hardcoded);
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

    /*  por si quieren probar algo sin qt
    std::string line;
    while (!in_game && std::getline(std::cin, line)) {
        int game_id = std::stoi(line);
        sendRequest(game_id);
        recvGame();
    }
    */
    return was_closed;
}

void Client::postGame(PlayerStats& player_stats) {
    int argc = 0;
    char** argv = nullptr;
    QApplication app(argc, argv);

    PostGameWindow win(player_stats);  // tu constructor recibe PlayerStats&
    win.show();

    app.exec();  // loop de eventos de la pantalla de estadísticas
}

void Client::recvGame() { // solo para probar cosas
    Op op = protocol.readActionByte();
    if (op != JOIN_GAME) {
        throw("em...");
    }
    JoinGame game_info = protocol.recvGameInfo();
    if (game_info.couldJoin()) {
        in_game = true;
    } else if (game_info.getExitStatus() == FULL_GAME) {
        std::cout << "GAME FULL" << std::endl;
    } else if (game_info.getExitStatus() == INEXISTENT_GAME) {
        std::cout << "INEXISTENT GAME" << std::endl;
    }
}

void Client::sendRequest(int game_id) { // solo para probar cosas
    auto rq = std::make_shared<RequestGame>(static_cast<ID>(game_id));
    CliMsgPtr base = rq;
    protocol.sendRequestGame(*rq);
}

void Client::stop(){
    sender.stop();
    receiver.stop();
    senderQueue.close();
    std::cout << "[Client] Sender y Receiver frenados" <<std::endl;
    close();
}

void Client::join(){
    receiver.join();
    std::cout << "[Client] Receiver joineado" <<std::endl;
    sender.join();
    std::cout << "[Client] Sender y Receiver joineados" <<std::endl;
}
void Client::close() { peer.close(); }

