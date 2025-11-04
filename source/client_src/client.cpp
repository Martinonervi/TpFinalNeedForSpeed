#include "client.h"

#include "../common_src/joingame.h"
#include "../common_src/requestgame.h"

#include "client_window.h"


Client::Client(const char* host, const char* service)
    : peer(host, service),
receiver(peer, receiverQueue), sender(peer, senderQueue)
{}

void Client::run() {
    sender.start();

    std::string line;
    std::getline(std::cin, line);

    int game_id = std::stoi(line);
    auto rq = std::make_shared<RequestGame>(static_cast<ID>(game_id));
    CliMsgPtr base = rq;
    std::cout << "[Client main] Mensaje de tipo: " << base->type() << "pusheado a la sender queue"<< std::endl;
    senderQueue.push(base);

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
        800,
        600,
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
}

void Client::join(){
    sender.join();
    receiver.join();
    close();
}
void Client::close() { peer.close(); }

