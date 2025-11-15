#include "client_window.h"

ClientWindow::ClientWindow(const int width, const int height, const std::string& title,
                           Queue<SrvMsgPtr>& receiverQueue, Queue<CliMsgPtr>& senderQueue):
        sdl(SDL_INIT_VIDEO),
        window(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
               SDL_WINDOW_SHOWN),
        renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        receiverQueue(receiverQueue),
        running(true),
        camera(width, height, 4640.0, 4672.0),  // Agregar consts
        myCarId(-1),
        tm(renderer),
        eventManager(myCarId, cars, renderer, senderQueue, tm, running, showMap) {}


// Hay que manejar FPS, hay que tener en cuenta los autos que no estan en camara
void ClientWindow::run() {
    Hud hud(renderer, tm, MAP_LIBERTY);
    Map map(renderer, tm, MAP_LIBERTY);
    while (running) {
        SrvMsgPtr srvMsg;
        while (receiverQueue.try_pop(srvMsg)) {
            eventManager.handleServerMessage(srvMsg);
        }

        eventManager.handleEvents();

        renderer.SetDrawColor(0, 0, 0, 255);
        renderer.Clear();
        map.draw(camera);

        for (auto& [id, car]: cars) {
            const auto carState = car->getState();
            if (id == myCarId) {
                camera.follow(car->getX(), car->getY());
            }
            switch (carState) {
                case ALIVE: {
                    car->draw(camera);
                    break;
                }
                case LOW_HEALTH: {
                    // DIBUJAMOS ROJO EN LA PANTALLA
                    break;
                }
                case DESTROYED: {
                    /*
                    cars.erase(id);
                    if (id == myCarId) {
                        myCarId = -1;
                    } */
                    break;
                }
                case EXPLODING: {
                    car->drawExplosion(camera);
                    break;
                }
                default: {
                    break;
                }
            }
        }
        if (showMap)
            hud.drawOverlay(window.GetWidth(), window.GetHeight(), cars, myCarId);  // Por ahora asi
        renderer.Present();
    }
    TTF_Quit();
    SDL_Quit();
}
