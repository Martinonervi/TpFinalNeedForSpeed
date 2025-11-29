#ifndef START_SCREEN_H
#define START_SCREEN_H

#include <SDL2pp/SDL2pp.hh>

#include "../../common_src/constants.h"
#include "../textures/texture_manager.h"

#include "minimap.h"
#include "sdl_drawer.h"
#include "upgrade_screen.h"


class StartScreen {
public:
    StartScreen(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, MapType mapType,
                std::vector<RecommendedPoint>& pathArray, UpgradeScreen& ups, Button& startBtn);
    void draw(int windowWidth, int windowHeight);

private:
    Minimap map;
    SdlDrawer& drawer;
    SDL2pp::Renderer& renderer;
    TextureManager& tm;
    UpgradeScreen& ups;
    Button& startBtn;

};



#endif //START_SCREEN_H
