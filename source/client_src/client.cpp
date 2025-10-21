#include "client.h"
#include <QApplication>
#include <iostream>
#include <sstream>
#include <string>
#include "../common_src/queue.h"


Client::Client(const char* host, const char* service)
    : peer(host, service),
receiver(peer, queue), sender(peer)
{}

void Client::Main() {
    sender.start();
    receiver.start();

    int fake_argc = 0;
    char **fake_argv = nullptr;
    QApplication app(fake_argc, fake_argv);
    ClientWindow client_window(queue);
    client_window.show();
    app.exec();

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
