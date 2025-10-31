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
    Queue<SrvMsg>& receiverQueue;
    Queue<CliMsg>& senderQueue;
    std::unordered_map<int, std::unique_ptr<Car>> cars;
    int myCarId;

    const std::unordered_map<SDL_Keycode, MoveInfo> keyToMove = {
        { SDLK_w, { 1, 0, 0, 0 } },
        { SDLK_s, { 0, 1, 0, 0 } },
        { SDLK_a, { 0, 0, -1, 0 } },
        { SDLK_d, { 0, 0,  1, 0 } },
        { SDLK_n, { 0, 0,  0, 1 } },
    };

    bool running;
    void handleEvents();
    void handleServerMessage(const SrvMsg& msg);
};

#endif // CLIENTWINDOW_H
