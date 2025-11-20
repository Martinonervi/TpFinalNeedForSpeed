#ifndef UPGRADE_SCREEN_H
#define UPGRADE_SCREEN_H

#include <SDL2pp/SDL2pp.hh>

#include "../textures/texture_manager.h"

#include "sdl_drawer.h"

class UpgradeScreen {
public:
    UpgradeScreen(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, int width, int height);
    void renderPopUp(int windowWidth, int windowHeight);
    void handleMouseMotion(int mouseX, int mouseY);
    bool handleMouseClick(int mouseX, int mouseY, std::string& clickedButton);

private:
    SDL2pp::Renderer& renderer;
    SdlDrawer& drawer;
    TextureManager& tm;
    Button buttons[3];

    int width;
    int height;
};



#endif //UPGRADE_SCREEN_H
