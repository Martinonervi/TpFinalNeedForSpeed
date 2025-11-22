#include "client_window.h"

#include "./renderables/checkpoint.h"
#include "renderables/hint.h"
#include "renderables/upgrade_screen.h"

ClientWindow::ClientWindow(const int width, const int height, const std::string& title,
                           Queue<SrvMsgPtr>& receiverQueue, Queue<CliMsgPtr>& senderQueue):
        sdl(SDL_INIT_VIDEO),
        window(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
               SDL_WINDOW_SHOWN),
        renderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        drawer(renderer),
        receiverQueue(receiverQueue),
        running(true),
        camera(width, height, 4640.0, 4672.0),  // Agregar consts
        myCarId(-1),
        nextCheckpoint(-1),
        tm(renderer),
        hint(renderer, tm, 0, 0),
        ups(renderer, drawer, tm, 500, 500),
        eventManager(myCarId, nextCheckpoint, cars, renderer, senderQueue, drawer,
            tm, checkpoints, hint, ups, showUpgradeMenu,running, showMap, quit,
            raceTime, raceNumber, playerStats) {}

// Hay que manejar FPS
std::pair<bool, std::unique_ptr<PlayerStats>> ClientWindow::run() {
    Hud hud(renderer, drawer, tm, MAP_LIBERTY);
    Map map(renderer, tm, MAP_LIBERTY);

    while (running) {
        SrvMsgPtr srvMsg;
        while (receiverQueue.try_pop(srvMsg)) {
            eventManager.handleServerMessage(srvMsg);
        }
        if (!running) {
            break;
        }


        eventManager.handleEvents();

        renderer.SetDrawColor(0, 0, 0, 255);
        renderer.Clear();
        map.draw(camera);

        if (nextCheckpoint != -1) {
            auto it = checkpoints.find(nextCheckpoint);
            if (it != checkpoints.end() && it->second) {
                hint.draw(camera);
            }
        }
        for (auto& [id, checkpoint] : checkpoints) {
            if (!checkpoint) continue;
            checkpoint->draw(camera);
        }
        for (auto& [id, car]: cars) {
            if (!car) continue;
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
            hud.drawOverlay(window.GetWidth(), window.GetHeight(), cars, myCarId, raceTime, raceNumber);  // Por ahora asi

        if (showUpgradeMenu) {
            ups.renderPopUp(window.GetWidth(), window.GetHeight());
        }
        //ups.renderPopUp(window.GetWidth(), window.GetHeight());
        map.drawOver(camera);
        renderer.Present();
    }
    TTF_Quit();
    SDL_Quit();

    return {quit, std::move(playerStats)};
}
