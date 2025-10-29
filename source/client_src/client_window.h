#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <string>
#include "../common_src/queue.h"
#include <SDL2pp/SDL2pp.hh>
#include "../common_src/constants.h"
#include "../client_src/renderables/car.h"

class ClientWindow {
public:
    ClientWindow(
        int width, int height,
        const std::string& title,
        Queue<SrvMsg>& receiverQueue,
        Queue<CliMsg>& senderQueue
        );

    void run();

 private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    TextureManager tm;
    Car playerCar;
    Queue<SrvMsg>& receiverQueue;
    Queue<CliMsg>& senderQueue;

    bool running;
    void handleEvents();

};

#endif // CLIENTWINDOW_H
