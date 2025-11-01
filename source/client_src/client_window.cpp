#include "client_window.h"

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Window.hh>

ClientWindow::ClientWindow(const int width, const int height, const std::string& title,
                           Queue<SrvMsgPtr>& receiverQueue, Queue<CliMsgPtr>& senderQueue):
        sdl(SDL_INIT_VIDEO),
        window(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
               SDL_WINDOW_SHOWN),
        renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        carsTexture(renderer, "../assets/cars/cars.png"),
        tm(carsTexture),
        receiverQueue(receiverQueue),
        senderQueue(senderQueue),
        running(true),
        myCarId(1) {
    cars[myCarId] = std::make_unique<Car>(renderer, tm, width/2,
                height/2, CAR_LIMO, 0);
}

// Hay que manejar FPS
void ClientWindow::run() {
    while (running) {
        SrvMsgPtr srvMsg;
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
                auto msg = std::make_shared<MoveMsg>(it->second);
                CliMsgPtr clientMsg = msg;
                senderQueue.push(clientMsg);
            }
        }
    }
}


void ClientWindow::handleServerMessage(const SrvMsgPtr msg) {
    switch (msg->type()) { // Get
       /* case InitPlayer:
            myCarId = msg.posicion.player_id;
            cars[myCarId] = std::make_unique<Car>(renderer, tm, msg.posicion.x,
                msg.posicion.y, CAR_PORSCHE, msg.posicion.angleRad);
            break;

        case NewCar:
            cars[msg.posicion.player_id] = std::make_unique<Car>(renderer, tm, msg.posicion.x,
                msg.posicion.y, CAR_PORSCHE, msg.posicion.angleRad);
            break;
*/
        case Movement: {
            const auto ps = dynamic_cast<const PlayerState&>(*msg);
            if (cars.count(ps.getPlayerId())) {
                std::cout << ps.getX() << std::endl;
                std::cout << ps.getY() << std::endl;
                std::cout << ps.getAngleRad() << std::endl;
                cars[ps.getPlayerId()]->update(ps.getX()*100, ps.getY()*100,
                    ps.getAngleRad());
            }
            break;
        }
/*
        case Disconnect:
            cars.erase(msg.posicion.player_id);
            if (msg.posicion.player_id == myCarId) {
                myCarId = -1;
                running = false;
            }
            break;
*/
        default:
            break;
    }
}


