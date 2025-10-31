#include "client.h"
#include "client_window.h"


Client::Client(const char* host, const char* service)
    : peer(host, service),
receiver(peer, receiverQueue), sender(peer, senderQueue)
{}

void Client::run() {
    sender.start();
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

