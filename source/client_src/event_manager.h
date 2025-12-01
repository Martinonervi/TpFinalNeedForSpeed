#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Window.hh>

#include "../common_src/cli_msg/cli_cheat_request.h"
#include "../common_src/cli_msg/cli_request_upgrade.h"
#include "../common_src/cli_msg/move_Info.h"
#include "../common_src/constants.h"
#include "../common_src/srv_msg/client_disconnect.h"
#include "../common_src/srv_msg/new_player.h"
#include "../common_src/srv_msg/player_state.h"
#include "../common_src/srv_msg/playerstats.h"
#include "../common_src/srv_msg/send_player.h"
#include "../common_src/srv_msg/srv_car_hit_msg.h"
#include "../common_src/srv_msg/srv_checkpoint_hit_msg.h"
#include "../common_src/srv_msg/srv_current_info.h"
#include "../common_src/srv_msg/srv_recommended_path.h"
#include "../common_src/srv_msg/srv_send_upgrade.h"
#include "../common_src/srv_msg/srv_upgrade_logic.h"
#include "renderables/car.h"
#include "renderables/checkpoint.h"
#include "renderables/hint.h"
#include "renderables/start_screen.h"
#include "renderables/upgrade_screen.h"

#include "audio_manager.h"

class EventManager {
public:
    EventManager(ID& myCarId, ID& nextCheckpoint, uint8_t& totalCheckpoints, ID& checkpointNumber,
                 std::unordered_map<ID, std::unique_ptr<Car>>& cars,
                 std::unordered_map<ID, std::unique_ptr<Car>>& npcs, SDL2pp::Renderer& renderer,
                 Queue<CliMsgPtr>& senderQueue, SdlDrawer& drawer, TextureManager& textureManager,
                 std::unordered_map<ID, std::unique_ptr<Checkpoint>>& checkpoints, Hint& hint,
                 UpgradeScreen& ups, Button& startBtn, bool& showScreen,
                 bool& running, bool& quit, int& raceTime, uint8_t& totalRaces,
                 uint8_t& raceNumber, std::unique_ptr<PlayerStats>& playerStats,
                 std::vector<RecommendedPoint>& pathArray,
                 std::vector<UpgradeDef>& upgradesArray, bool& srvDisconnect, StartScreen& startScreen,
                 int& countdown, uint8_t& ranking);

    void handleEvents(AudioManager& audio) const;
    void handleServerMessage(const SrvMsgPtr& msg, AudioManager& audio);

private:
    ID& myCarId;
    ID& nextCheckpoint;
    uint8_t& totalCheckpoints;
    uint8_t& ranking;
    ID& checkpointNumber;
    std::unordered_map<ID, std::unique_ptr<Car>>& cars;
    std::unordered_map<ID, std::unique_ptr<Car>>& npcs;
    std::unordered_map<ID, std::unique_ptr<Checkpoint>>& checkpoints;
    SDL2pp::Renderer& renderer;
    Queue<CliMsgPtr>& senderQueue;
    SdlDrawer& drawer;
    TextureManager& tm;
    Hint& hint;
    UpgradeScreen& ups;
    Button& startBtn;
    bool& showScreen;
    bool& running;
    bool& quit;
    std::unique_ptr<PlayerStats>& playerStats;
    int& raceTime;
    uint8_t& totalRaces;
    uint8_t& raceNumber;
    uint8_t lastRaceNumber = 0x0;
    std::vector<RecommendedPoint>& pathArray;
    std::vector<UpgradeDef>& upgradesArray;
    bool& srvDisconnect;
    StartScreen& startScreen;
    int& countdown;

    const std::unordered_map<SDL_Keycode, MoveMsg> keyToMove = {
            {SDLK_w, MoveMsg(1, 0, 0, 0)},  {SDLK_s, MoveMsg(2, 0, 0, 0)},
            {SDLK_a, MoveMsg(0, 0, -1, 0)}, {SDLK_d, MoveMsg(0, 0, 1, 0)},
            {SDLK_n, MoveMsg(0, 0, 0, 1)},  {SDLK_SPACE, MoveMsg(0, 1, 0, 0)},
    };

    const std::unordered_map<SDL_Keycode, CheatRequest> keyToCheat = {
        {SDLK_t, CheatRequest(HEALTH_CHEAT)},  {SDLK_r, CheatRequest(WIN_RACE_CHEAT)},
        {SDLK_u, CheatRequest(FREE_SPEED_CHEAT)}, {SDLK_c, CheatRequest(LOST_RACE_CHEAT)},
        {SDLK_o, CheatRequest(NEXT_CHECKPOINT_CHEAT)},
    };
};


#endif //EVENT_MANAGER_H
