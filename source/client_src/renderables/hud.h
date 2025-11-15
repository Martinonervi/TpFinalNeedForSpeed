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
    void drawSpeed(SDL2pp::Renderer& renderer, float speed, int windowHeight,
                   int windowWidth) const;
    void drawBars(SDL2pp::Renderer& renderer, int windowWidth) const;
    void drawHealthFill(SDL2pp::Renderer& renderer, float percent, SDL2pp::Rect healthDst,
                        int scale) const;
    void drawEnergyFill(SDL2pp::Renderer& renderer, float percent, SDL2pp::Rect energyDst,
                        int scale) const;
};



#endif
