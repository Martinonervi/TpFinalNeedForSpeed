#include "client.h"

#include <iostream>
#include <sstream>
#include <string>

Client::Client(const char* host, const char* service)
    : peer(host, service), receiver(peer), sender(peer) {}

void Client::Main() {
    sender.start();
    receiver.start();

    while (sender.is_listening()) {

    }
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
