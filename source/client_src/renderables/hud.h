#ifndef HUD_H
#define HUD_H
#include <SDL2pp/SDL2pp.hh>

#include "../../common_src/constants.h"
#include "../../common_src/srv_msg/srv_recommended_path.h"
#include "../renderables/car.h"
#include "../textures/texture_manager.h"

#include "minimap.h"
#include "sdl_drawer.h"


class Hud {
public:
    Hud(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, MapType mapType,
        std::vector<RecommendedPoint>& pathArray);
    void drawOverlay(int x, int y, std::unordered_map<ID, std::unique_ptr<Car>>& cars, ID playerId,
    int raceTime, uint8_t totalRaces, uint8_t raceNumber,
    uint8_t totalCheckpoints, ID checkpointNumber) const;

private:
    Minimap map;
    SdlDrawer& drawer;
    SDL2pp::Renderer& renderer;
    TextureManager& tm;

    void drawDial(SDL2pp::Renderer& renderer, float speed, int windowWidth,
                   int windowHeight) const;
    float drawNeedle(float x, float y, SDL2pp::Rect dstRectDial, float speed) const;
    void drawSpeedText(float clampedSpeed, SDL2pp::Rect dstRectDial, float x) const;
    void drawBars(SDL2pp::Renderer& renderer, int windowWidth, float healthPerc) const;
    void drawHealthFill(SDL2pp::Renderer& renderer, float healthPerc, SDL2pp::Rect healthDst,
                        int scale) const;
    void drawGameTime(int totalSeconds) const;
    void drawRaceNumber(int current, int total) const;
    void drawCheckpointNumber(int current, int total) const;
    void activeUpgrades(int windowWidth, const std::vector<Upgrade>& upgrades) const;
    void drawUpgrade(int windowWidth, Upgrade upgrade, int i) const;
};



#endif
