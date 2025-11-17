#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Texture.hh>
#include <SDL2pp/Window.hh>

#include "../common_src/cli_msg/move_Info.h"
#include "../common_src/constants.h"
#include "../common_src/srv_msg/new_player.h"
#include "../common_src/srv_msg/player_state.h"
#include "../common_src/srv_msg/send_player.h"
#include "../common_src/srv_msg/srv_car_hit_msg.h"
#include "renderables/car.h"
#include "renderables/checkpoint.h"
#include "renderables/hint.h"

class EventManager {
public:
    EventManager(ID& myCarId, ID& nextCheckpoint,
    std::unordered_map<ID, std::unique_ptr<Car>>& cars,
                              SDL2pp::Renderer& renderer,
                              Queue<CliMsgPtr>& senderQueue, TextureManager& textureManager,
                              std::unordered_map<ID, std::unique_ptr<Checkpoint>>& checkpoints,
                              Hint& hint,
                              bool& running, bool& showMap, bool& quit);

    void handleEvents() const;
    void handleServerMessage(const SrvMsgPtr& msg) const;

private:
    ID& myCarId;
    ID& nextCheckpoint;
    std::unordered_map<ID, std::unique_ptr<Car>>& cars;
    std::unordered_map<ID, std::unique_ptr<Checkpoint>>& checkpoints;
    SDL2pp::Renderer& renderer;
    Queue<CliMsgPtr>& senderQueue;
    TextureManager& tm;
    Hint& hint;
    bool& running;
    bool& showMap;
    bool& quit;


    const std::unordered_map<SDL_Keycode, MoveMsg> keyToMove = {
            {SDLK_w, MoveMsg(1, 0, 0, 0)},  {SDLK_s, MoveMsg(2, 0, 0, 0)},
            {SDLK_a, MoveMsg(0, 0, -1, 0)}, {SDLK_d, MoveMsg(0, 0, 1, 0)},
            {SDLK_n, MoveMsg(0, 0, 0, 1)},  {SDLK_SPACE, MoveMsg(0, 1, 0, 0)},
    };

};



#endif //EVENT_MANAGER_H
