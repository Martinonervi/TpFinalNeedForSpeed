#ifndef HUD_H
#define HUD_H
#include "minimap.h"

#include <SDL2pp/SDL2pp.hh>

#include "../../common_src/constants.h"
#include "../renderables/car.h"
#include "../textures/texture_manager.h"


class Hud {
public:
    Hud(SDL2pp::Renderer& renderer, TextureManager& tm, MapType mapType);
    void drawOverlay(int x, int y, std::unordered_map<ID, std::unique_ptr<Car>>& cars, ID playerId);

private:
    Minimap map;
    SDL2pp::Renderer& renderer;
    TextureManager& tm;
    TTF_Font* font;

    void loadFont();
    void drawDial(SDL2pp::Renderer& renderer, float speed, int windowWidth,
                   int windowHeight) const;
    float drawNeedle(float x, float y, SDL2pp::Rect dstRectDial, float speed) const;
    void drawSpeedText(float clampedSpeed, SDL2pp::Rect dstRectDial, float x) const;
    void drawBars(SDL2pp::Renderer& renderer, int windowWidth, float health) const;
    void drawHealthFill(SDL2pp::Renderer& renderer, float health, SDL2pp::Rect healthDst,
                        int scale) const;
    void drawEnergyFill(SDL2pp::Renderer& renderer, float percent, SDL2pp::Rect energyDst,
                        int scale) const;
    void drawGameTime(int totalSeconds) const;
    void drawRaceNumber(int current, int total) const;
    void drawText(const std::string& text, int x, int y, SDL_Color color) const;
};



#endif
