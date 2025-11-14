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
    void drawHealthBar(SDL2pp::Renderer& renderer, int windowWidth, float healthPercent) const;
    void drawSpeed(SDL2pp::Renderer& renderer, float speed, int windowHeight, int windowWidth) const;
};



#endif
