#include "client_window.h"

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Window.hh>

#include "../common_src/send_player.h"

ClientWindow::ClientWindow(const int width, const int height, const std::string& title,
                           Queue<SrvMsgPtr>& receiverQueue, Queue<CliMsgPtr>& senderQueue):
        sdl(SDL_INIT_VIDEO),
        window(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
               SDL_WINDOW_SHOWN),
        renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        carsTexture(renderer, "../assets/cars/cars.png"),
        mapsTexture(renderer, "../assets/cities/LibertyCity.png"),
        tm(carsTexture, mapsTexture),
        receiverQueue(receiverQueue),
        senderQueue(senderQueue),
        running(true),
        camera(width, height, 4640, 4672), // Agregar consts
        myCarId(-1)
{}

// Hay que manejar FPS
void ClientWindow::run() {
    const Map map(renderer, tm);
    while (running) {
        SrvMsgPtr srvMsg;
        while (receiverQueue.try_pop(srvMsg)) {
            handleServerMessage(srvMsg);
        }

        handleEvents();

        renderer.SetDrawColor(0, 0, 0, 0);
        renderer.Clear();
        map.draw(camera);

        for (auto& [id, car] : cars) {
            if (id == myCarId) {
                camera.follow(car->getX(), car->getY()); // Problemas float/ int
            }
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


void ClientWindow::handleServerMessage(const SrvMsgPtr& msg) {
    switch (msg->type()) {
        case INIT_PLAYER: {
            const auto sp = dynamic_cast<const SendPlayer&>(*msg);
            myCarId = sp.getPlayerId();
            std::cout << "Bienvenido Player:" << myCarId << std::endl;
            cars[myCarId] = std::make_unique<Car>(renderer, tm, sp.getX(),
                sp.getY(), sp.getCarType(), sp.getAngleRad());
            break;
        }
        /*case NEW_CAR: {
            const auto snc = dynamic_cast<const SendNewCar&>(*msg);
            std::cout << "Se Unio Player:" << snc.getPlayerId() << std::endl;
            cars[snc.getPlayerId()] = std::make_unique<Car>(renderer, tm, snc.getX(),
                snc.getY(), snc.getCarType(), snc.getAngleRad());
            break;
        }*/
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


