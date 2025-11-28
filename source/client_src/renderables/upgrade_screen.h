#ifndef UPGRADE_SCREEN_H
#define UPGRADE_SCREEN_H

#include <SDL2pp/SDL2pp.hh>

#include "../textures/texture_manager.h"

#include "sdl_drawer.h"
#include "button.h"

struct UpgradeButton {
    Button button;
    Upgrade type;
};

class UpgradeScreen {
public:
    UpgradeScreen(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, int width,
                  int height, int windowWidth, int windowHeight);
    void renderPopUp();
    void handleMouseMotion(int mouseX, int mouseY);
    std::pair<bool, Upgrade> handleMouseClick();
    void createButtons(const std::vector<UpgradeDef>& upgradesArray);

private:
    SDL2pp::Renderer& renderer;
    SdlDrawer& drawer;
    TextureManager& tm;
    int windowWidth;
    int windowHeight;

    std::vector<UpgradeButton> buttons;

    int width;
    int height;
};



#endif //UPGRADE_SCREEN_H
