#include "client_window.h"

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Window.hh>

ClientWindow::ClientWindow(const int width, const int height, const std::string& title,
                           Queue<SrvMsg>& receiverQueue, Queue<CliMsg>& senderQueue):
        sdl(SDL_INIT_VIDEO),
        window(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
               SDL_WINDOW_SHOWN),
        renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        tm(renderer),
        receiverQueue(receiverQueue),
        senderQueue(senderQueue),
        running(true),
        myCarId(-1)
{}

// Tengo que tener en cuenta la camara a la hora de dibujar, voy a recibir una posicion x,y que
// es relativa y la transformo en pixeles (ya lo hare)
void ClientWindow::run() {
    while (running) {
        SrvMsg srvMsg;
        while (receiverQueue.try_pop(srvMsg)) {
            handleServerMessage(srvMsg);
        }

        handleEvents();

        renderer.SetDrawColor(0, 128, 0, 255);
        renderer.Clear();

        for (auto& [id, car] : cars) {
            car->draw();
        }

        renderer.Present();
        SDL_Delay(16);
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}


// Esto podria ir en un archivo aparte (clase event_manager o algo por el estilo)
void ClientWindow::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN && myCarId != -1) {
            auto it = keyToMove.find(event.key.keysym.sym);
            if (it != keyToMove.end()) {
                CliMsg clientMsg{};
                clientMsg.player_id = myCarId;
                clientMsg.event_type = Movement;
                clientMsg.movement = it->second;
                senderQueue.push(clientMsg);
            }
        }
    }
}


// Podria ser un hash
void ClientWindow::handleServerMessage(const SrvMsg& msg) {
    switch (msg.type) {
        case InitPlayer:
            myCarId = msg.posicion.player_id;
            cars[myCarId] = std::make_unique<Car>(renderer, tm, msg.posicion.x,
                msg.posicion.y, CAR_PORSCHE, msg.posicion.angleRad);
            break;

        case NewCar:
            cars[msg.posicion.player_id] = std::make_unique<Car>(renderer, tm, msg.posicion.x,
                msg.posicion.y, CAR_PORSCHE, msg.posicion.angleRad);
            break;

        case Movement:
            if (cars.count(msg.posicion.player_id)) {
                cars[msg.posicion.player_id]->update(msg.posicion.x, msg.posicion.y,
                    msg.posicion.angleRad);
            }
            break;

        case Disconnect:
            cars.erase(msg.posicion.player_id);
            if (msg.posicion.player_id == myCarId) {
                myCarId = -1;
                running = false;
            }
            break;

        default:
            break;
    }
}

