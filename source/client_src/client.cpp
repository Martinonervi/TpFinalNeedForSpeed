#include "client.h"

#include "../common_src/joingame.h"
#include "../common_src/requestgame.h"

#include "client_window.h"
#include <QApplication>
#include "lobby/lobbywindow.h"

Client::Client(const char* host, const char* service)
    : peer(host, service), protocol(peer),
receiver(protocol, receiverQueue), sender(protocol, senderQueue)
{}

void Client::lobbyState() {
    /*
    int argc = 0;
    char** argv = nullptr;
    QApplication app(argc, argv);

    LobbyWindow window(protocol);

    window.show();

    app.exec();   // event loop de Qt
    */
    std::string line;
    while (!in_game && std::getline(std::cin, line)) {
        int game_id = std::stoi(line);
        sendRequest(game_id);
        recvGame();
    }


}

void Client::recvGame() {
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

void Client::sendRequest(int game_id) {
    auto rq = std::make_shared<RequestGame>(static_cast<ID>(game_id));
    CliMsgPtr base = rq;
    protocol.sendRequestGame(*rq);
}

void Client::run() {

    std::string line;
    lobbyState();

    sender.start();
    receiver.start();

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

    ClientWindow client_window(
        1200,
        800,
        "Need For Speed",
        receiverQueue,
        senderQueue
    );

    client_window.run();
    stop();
    join();
}

void Client::stop(){
    sender.stop();
    receiver.stop();
    senderQueue.close();
    std::cout << "[Client] Sender y Receiver frenados" <<std::endl;
}

void Client::join(){
    receiver.join();
    std::cout << "[Client] Receiver joineado" <<std::endl;
    sender.join();
    std::cout << "[Client] Sender y Receiver joineados" <<std::endl;
    close();
}
void Client::close() { peer.close(); }

