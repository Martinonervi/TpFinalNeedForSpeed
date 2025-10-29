#include "client_window.h"

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Window.hh>

ClientWindow::ClientWindow(
    const int width,
    const int height,
    const std::string& title,
    Queue<SrvMsg>& receiverQueue,
    Queue<CliMsg>& senderQueue
    )
    : sdl(SDL_INIT_VIDEO),
      window(
          title,
          SDL_WINDOWPOS_CENTERED,
          SDL_WINDOWPOS_CENTERED,
          width,
          height,
          SDL_WINDOW_SHOWN
          ),
      renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
      tm(renderer),
      playerCar(
          renderer,
          tm,
          width/2,
          height/2,
          CAR_PORSCHE,
          0
          ),
      receiverQueue(receiverQueue),
      senderQueue(senderQueue),
      running(true)
{}

// Hay que arreglarlo teniendo en cuenta el manejo de autos
void ClientWindow::run() {
    while (running) {
        SrvMsg srvMsg;
        while (receiverQueue.try_pop(srvMsg)) {
            if (srvMsg.type == Movement) {
                playerCar.draw(
                    static_cast<int>(srvMsg.posicion.vy),
                    static_cast<int>(srvMsg.posicion.vx)
                    );
            }
        }
        handleEvents();
        renderer.SetDrawColor(0, 128, 0, 255);
        renderer.Clear();
        renderer.Present();
        SDL_Delay(16);
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}

void ClientWindow::handleEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            running = false;
        } else if(event.type == SDL_KEYDOWN) {
            MoveInfo moveInfo{};
            CliMsg clientMsg{};
            switch(event.key.keysym.sym) {
                case SDLK_w:
                    moveInfo.accelerate = 1;
                    moveInfo.brake = 0;
                    moveInfo.steer = 0;
                    moveInfo.nitro = 0;

                    clientMsg.event_type = Movement;
                    clientMsg.movement = moveInfo;
                    senderQueue.push(clientMsg);

                    break;
                case SDLK_s:
                    moveInfo.accelerate = 0;
                    moveInfo.brake = 1;
                    moveInfo.steer = 0;
                    moveInfo.nitro = 0;

                    clientMsg.event_type = Movement;
                    clientMsg.movement = moveInfo;
                    senderQueue.push(clientMsg);

                    break;
                case SDLK_a:
                    moveInfo.accelerate = 0;
                    moveInfo.brake = 0;
                    moveInfo.steer = -1;
                    moveInfo.nitro = 0;

                    clientMsg.event_type = Movement;
                    clientMsg.movement = moveInfo;
                    senderQueue.push(clientMsg);

                    break;
                case SDLK_d:
                    moveInfo.accelerate = 0;
                    moveInfo.brake = 0;
                    moveInfo.steer = 1;
                    moveInfo.nitro = 0;

                    clientMsg.event_type = Movement;
                    clientMsg.movement = moveInfo;
                    senderQueue.push(clientMsg);

                    break;
                default: break;
            }
        }
    }
}



