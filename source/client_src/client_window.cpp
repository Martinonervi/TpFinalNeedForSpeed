#include "client_window.h"

#include "./renderables/checkpoint.h"
#include "renderables/hint.h"
#include "renderables/upgrade_screen.h"

#include "audio_manager.h"

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
        ups(renderer, drawer, tm, 500, 500, window.GetWidth(), window.GetHeight()),
        eventManager(myCarId, nextCheckpoint, cars, renderer, senderQueue, drawer,
            tm, checkpoints, hint, ups, showUpgradeMenu,running, showMap, quit,
            raceTime, totalRaces, raceNumber, playerStats, pathArray, upgrade, upgradesArray) {}

// Hay que manejar FPS
std::pair<bool, std::unique_ptr<PlayerStats>> ClientWindow::run() {
    Hud hud(renderer, drawer, tm, MAP_LIBERTY, pathArray);
    Map map(renderer, tm, MAP_LIBERTY);
    AudioManager audio;
    audio.loadSound("explosion", "../assets/sfx/explosion.wav");
    audio.loadSound("crash", "../assets/sfx/crash.mp3"); // PASAR A WAV
    audio.loadSound("checkpoint", "../assets/sfx/checkpoint.wav");
    audio.loadSound("countdown", "../assets/sfx/countdown.wav");
    audio.loadSound("engine", "../assets/sfx/engine.wav");
    audio.loadSound("purchase", "../assets/sfx/purchase.wav");

    audio.loadMusic("game_music", "../assets/sfx/game-music.wav");

    audio.playMusic("game_music", -1);
    while (running) {


        SrvMsgPtr srvMsg;
        while (receiverQueue.try_pop(srvMsg)) {
            eventManager.handleServerMessage(srvMsg, audio);
        }
        if (!running) {
            break;
        }


        eventManager.handleEvents();

        renderer.SetDrawColor(0, 0, 0, 255);
        renderer.Clear();
        map.draw(camera);

        for (auto& [id, checkpoint] : checkpoints) {
            if (!checkpoint) continue;
            checkpoint->draw(camera);
        }
        for (auto& [id, car]: cars) {
            if (!car) continue;
            const auto carState = car->getState();
            if (id == myCarId) {
                if (car->getSpeed() > 0) audio.playSound("engine", -1);
                else audio.stopSound("engine");
                camera.follow(car->getX(), car->getY());
            }
            switch (carState) {
                case ALIVE: {
                    car->draw(camera);
                    break;
                }
                case LOW_HEALTH: {
                    car->draw(camera);
                    const uint32_t ticks = SDL_GetTicks();
                    const float intensity = (sin(ticks * 0.005f) + 1.0f) * 0.5f;
                    const auto alpha = static_cast<uint8_t>(intensity * 120);

                    SDL_Rect full = {0, 0, window.GetWidth(), window.GetHeight()};

                    renderer.SetDrawColor(255, 0, 0, alpha);
                    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
                    renderer.FillRect(full);
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
		map.drawOver(camera);

		if (nextCheckpoint != -1) {
            auto it = checkpoints.find(nextCheckpoint);
            if (it != checkpoints.end() && it->second) {
                hint.draw(camera);
            }
        }

        if (showMap)
            hud.drawOverlay(window.GetWidth(), window.GetHeight(), cars, myCarId, raceTime, totalRaces, raceNumber, upgrade);  // Por ahora asi

        if (showUpgradeMenu) {
            ups.renderPopUp();
        }

        renderer.Present();
    }
    audio.stopMusic();
    TTF_Quit();
    SDL_Quit();

    return {quit, std::move(playerStats)};
}
