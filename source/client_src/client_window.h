#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <string>

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Window.hh>
#include <unistd.h>

#include "../common_src/cli_msg/move_Info.h"
#include "../common_src/constants.h"
#include "../common_src/queue.h"
#include "../common_src/srv_msg/player_state.h"
#include "../common_src/srv_msg/playerstats.h"
#include "renderables/car.h"
#include "renderables/checkpoint.h"
#include "renderables/hud.h"
#include "renderables/map.h"
#include "renderables/sdl_drawer.h"
#include "textures/texture_manager.h"

#include "client_camera.h"
#include "event_manager.h"
#include "../common_src/constant_rate_loop.h"
#include "./renderables/checkpoint.h"
#include "renderables/hint.h"
#include "renderables/start_screen.h"
#include "renderables/upgrade_screen.h"

#include "audio_manager.h"
#include "sdl_constants.h"

class ClientWindow {
public:
    ClientWindow(
        int width, int height,
        const std::string& title,
        Queue<SrvMsgPtr>& receiverQueue,
        Queue<CliMsgPtr>& senderQueue
        );

    std::pair<bool, std::unique_ptr<PlayerStats>> run();

private:
    SDL2pp::SDL sdl;
    SDL2pp::Window window;
    SDL2pp::Renderer renderer;
    SdlDrawer drawer;
    TextureManager tm;
    Queue<SrvMsgPtr>& receiverQueue;
    std::unordered_map<ID, std::unique_ptr<Car>> cars;
    std::unordered_map<ID, std::unique_ptr<Checkpoint>> checkpoints;
    Camera camera;
    ID myCarId;
    ID nextCheckpoint;
    uint8_t totalCheckpoints = DEFAULT;
    ID checkpointNumber = DEFAULT;
    Hint hint;
    std::unique_ptr<PlayerStats> playerStats = nullptr;
    UpgradeScreen ups;
    int raceTime = DEFAULT;
    uint8_t totalRaces = DEFAULT;
    uint8_t raceNumber = DEFAULT;
    Button startBtn;
    EventManager eventManager;
    bool running;
    bool quit = false;
    bool showStart = true;
    std::vector<RecommendedPoint> pathArray;
    std::vector<UpgradeDef> upgradesArray;
    AudioManager audio;


    void drawCars(ID id, const std::unique_ptr<Car>& car);
    void carHasLowHealth();
};

#endif // CLIENTWINDOW_H
