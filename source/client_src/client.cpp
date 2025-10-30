#include "client.h"


Client::Client(const char* host, const char* service)
    : peer(host, service),
receiver(peer, receiverQueue), sender(peer, senderQueue)
{}

void Client::run() {
    sender.start();
    receiver.start();

    /*ClientWindow client_window(
        800,
        600,
        "Need For Speed",
        "../assets/cars/cars.png",
        receiverQueue,
        senderQueue
    );

    client_window.run();
*/  while (true) {};
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

