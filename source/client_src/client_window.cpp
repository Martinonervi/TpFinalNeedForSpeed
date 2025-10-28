#include "client_window.h"
#include <SDL2pp/SDL.hh>
#include <SDL2pp/Window.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

ClientWindow::ClientWindow(
    const int width,
    const int height,
    const std::string& title,
    const std::string& carImagePath
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
      carTexture(renderer, carImagePath),
      playerCar(
          carTexture,
          renderer,
          CAR_PORSCHE,
          width/2,
          height/2,
          5
          ),
      running(true)
{}


void ClientWindow::run() {
    while (running) {
        handleEvents();
        renderer.SetDrawColor(0, 128, 0, 255);
        renderer.Clear();
        playerCar.draw();
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
            switch(event.key.keysym.sym) {
                case SDLK_UP:    playerCar.move(0, -1); break;
                case SDLK_DOWN:  playerCar.move(0, 1); break;
                case SDLK_LEFT:  playerCar.move(-1, 0); break;
                case SDLK_RIGHT: playerCar.move(1, 0); break;
                case SDLK_ESCAPE: running = false; break;
                default: break;
            }
        }
    }
}


