#ifndef UPGRADE_SCREEN_H
#define UPGRADE_SCREEN_H

#include <SDL2pp/SDL2pp.hh>
#include "../textures/texture_manager.h"
#include <unordered_map>
#include "sdl_drawer.h"
#include "button.h"

struct UpgradeButton {
    Button button;
    Upgrade type;
    float penalty;
    SDL2pp::Rect iconRect;
};

class UpgradeScreen {
public:
    UpgradeScreen(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, int width,
                  int height, int windowWidth, int windowHeight);
    void renderPopUp() const;

    void handleMouseMotion(int mouseX, int mouseY);
    std::pair<bool, Upgrade> handleMouseClick();

    void createButtons(const std::vector<UpgradeDef>& upgradesArray);
    void changeState(Upgrade upgrade);


private:
    SDL2pp::Renderer& renderer;
    SdlDrawer& drawer;
    TextureManager& tm;
    int windowWidth;
    int windowHeight;

    std::vector<UpgradeButton> buttons;

    int width;
    int height;

    void writeDescription(float penalty, const std::string& name, int x, int y) const;

    const std::unordered_map<Upgrade, std::string> upgradeDescriptions = {
        { HEALTH, "Increases the car's health" },
        { ENGINE_FORCE, "Boosts acceleration" },
        { SHIELD, "Adds collision resistance" },
        { DAMAGE, "Increases damage on impact" },
    };

};



#endif //UPGRADE_SCREEN_H
