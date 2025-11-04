#include "client.h"
#include "client_window.h"


Client::Client(const char* host, const char* service)
    : peer(host, service),
receiver(peer, receiverQueue), sender(peer, senderQueue)
{}

void Client::run() {
    sender.start();


    std::string line;
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
    receiver.start();


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

