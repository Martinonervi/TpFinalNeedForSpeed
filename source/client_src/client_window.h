#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <string>
#include "../common_src/queue.h"
#include <SDL2pp/SDL2pp.hh>
#include "../common_src/constants.h"
#include "../common_src/car.h"

class ClientWindow {
public:
    ClientWindow(
        int width, int height,
        const std::string& title,
        const std::string& carImagePath,
        Queue<constants::SrvMsg>& receiverQueue,
        Queue<constants::CliMsg>& senderQueue
        );

    void run();

 private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SDL2pp::Texture carTexture;
    Car playerCar;
    Queue<constants::SrvMsg>& receiverQueue;
    Queue<constants::CliMsg>& senderQueue;

    bool running;
    void handleEvents();

};

#endif // CLIENTWINDOW_H
