#include "client_window.h"

ClientWindow::ClientWindow(const int width, const int height, const std::string& title,
                           Queue<SrvMsgPtr>& receiverQueue, Queue<CliMsgPtr>& senderQueue):
        sdl(SDL_INIT_VIDEO),
        window(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
               SDL_WINDOW_SHOWN),
        renderer(window, NOT_ACCESSIBLE, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        drawer(renderer),
        receiverQueue(receiverQueue),
        running(true),
        camera(static_cast<float>(width), static_cast<float>(height),
            WORLD_WIDTH, WORLD_HEIGHT),
        myCarId(NOT_ACCESSIBLE),
        nextCheckpoint(NOT_ACCESSIBLE),
        tm(renderer),
        hint(renderer, tm, DEFAULT, DEFAULT),
        ups(renderer, drawer, tm, UPS_WIDTH, UPS_HEIGHT,
            window.GetWidth(), window.GetHeight()),
        startBtn(
            {
                (window.GetWidth() - START_BTN_WIDTH)/2,
                window.GetHeight()/ 100 * START_BTN_HEIGHT,
                START_BTN_WIDTH, START_BTN_HEIGHT
            },
            BTN_TEXT, BTN_GREEN, BTN_GREEN_HOVER ),
        disScreen(window.GetWidth(), window.GetHeight(), renderer, drawer),
        startScreen(renderer, drawer, tm, MAP_LIBERTY, pathArray, ups, startBtn),
        eventManager(myCarId, nextCheckpoint, totalCheckpoints, checkpointNumber,
            cars, renderer, senderQueue, drawer, tm, checkpoints, hint,
            ups, startBtn, showScreen, running, quit, raceTime, totalRaces,
            raceNumber, playerStats, pathArray, upgradesArray, srvDisconnect, startScreen)
{}

std::pair<bool, std::unique_ptr<PlayerStats>> ClientWindow::run() {
    const Hud hud(renderer, drawer, tm, MAP_LIBERTY, pathArray);
    Map map(renderer, tm, MAP_LIBERTY);

    bool lastShowScreen = false;

    try {
        ConstantRateLoop loop(FPS);
        audio.playMusic("game_music", LOOP);
        while (running) {
            SrvMsgPtr srvMsg;
            while (receiverQueue.try_pop(srvMsg)) {
                eventManager.handleServerMessage(srvMsg, audio);
            }

            if (!running) break;

            eventManager.handleEvents(audio);

            renderer.SetDrawColor(BLACK);
            renderer.Clear();

            if (showScreen) {
                audio.stopSound("engine");
                if (!lastShowScreen) {
                    cars[myCarId]->clearUpgrades();
                    for (auto& [id, car]: cars) {
                        car->setState(ALIVE);
                        if (id == myCarId) {
                            car->resetStats();
                        }
                    }
                }
                startScreen.draw(window.GetWidth(), window.GetHeight());
            } else {
                map.draw(camera);

                for (auto& [id, checkpoint] : checkpoints) {
                    if (!checkpoint) continue;
                    checkpoint->draw(camera);
                }

                for (auto& [id, car]: cars) {
                    if (!car) continue;
                    drawCars(id, car);
                }
                map.drawOver(camera);

                if (nextCheckpoint != NOT_ACCESSIBLE) {
                    auto it = checkpoints.find(nextCheckpoint);
                    if (it != checkpoints.end() && it->second) {
                        hint.draw(camera);
                    }
                }

                hud.drawOverlay(window.GetWidth(), window.GetHeight(),
                    cars, myCarId, raceTime, totalRaces, raceNumber,
                    totalCheckpoints, checkpointNumber);

            }
            lastShowScreen = showScreen;

            renderer.Present();
            loop.sleep_until_next_frame();
        }

        audio.stopMusic();
        uint32_t start = SDL_GetTicks();
        while (SDL_GetTicks() - start < 3000 && srvDisconnect) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    srvDisconnect = false;
                }
            }

            renderer.SetDrawColor(BLACK);
            renderer.Clear();

            map.draw(camera);
            disScreen.draw();

            renderer.Present();
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    TTF_Quit();
    SDL_Quit();

    return {quit, std::move(playerStats)};
}


void ClientWindow::drawCars(const ID id, const std::unique_ptr<Car>& car) {
    const auto carState = car->getState();

    if (id == myCarId) {
        if (car->getSpeed() > 0) audio.playSound("engine", LOOP);
        else audio.stopSound("engine");
        camera.follow(car->getX(), car->getY());
    }

    switch (carState) {
        case ALIVE: {
            car->draw(camera);
            break;
        }
        case LOW_HEALTH: {
            if (id != myCarId) break;
            carHasLowHealth();
            break;
        }
        case DESTROYED: {
            nextCheckpoint = NOT_ACCESSIBLE;
            /*
            cars.erase(id);
            if (id == myCarId) {
                myCarId = NOT_ACCESSIBLE;
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

void ClientWindow::carHasLowHealth() {
    cars[myCarId]->draw(camera);
    const uint32_t ticks = SDL_GetTicks();
    const float intensity = (sin(ticks * 0.005f) + 1.0f) * 0.5f;
    const auto alpha = static_cast<uint8_t>(intensity * 120);

    const SDL2pp::Rect full = {0, 0, window.GetWidth(), window.GetHeight()};

    renderer.SetDrawColor(RED.r, RED.g, RED.b, alpha);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.FillRect(full);
}