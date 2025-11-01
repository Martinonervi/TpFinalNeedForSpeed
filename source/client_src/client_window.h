#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <string>
#include "../common_src/queue.h"
#include <SDL2pp/SDL2pp.hh>
#include "../common_src/constants.h"
#include "renderables/car.h"
#include "textures/texture_manager.h"

class ClientWindow {
public:
    ClientWindow(
        int width, int height,
        const std::string& title,
        Queue<SrvMsgPtr>& receiverQueue,
        Queue<CliMsgPtr>& senderQueue
        );

    void run();

private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SDL2pp::Texture carsTexture;
    TextureManager tm;
    Queue<SrvMsgPtr>& receiverQueue;
    Queue<CliMsgPtr>& senderQueue;
    std::unordered_map<int, std::unique_ptr<Car>> cars;
    int myCarId;

    const std::unordered_map<SDL_Keycode, MoveMsg> keyToMove = {
        { SDLK_w, MoveMsg( 1, 0, 0, 0 ) },
        { SDLK_s, MoveMsg( 0, 1, 0, 0 ) },
        { SDLK_a, MoveMsg( 0, 0, -1, 0 ) },
        { SDLK_d, MoveMsg( 0, 0,  1, 0 ) },
        { SDLK_n, MoveMsg( 0, 0,  0, 1 ) },
    };

    bool running;
    void handleEvents();
    void handleServerMessage(SrvMsgPtr msg);
};

#endif // CLIENTWINDOW_H
