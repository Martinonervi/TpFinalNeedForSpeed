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
};

class UpgradeScreen {
public:
    UpgradeScreen(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, int width,
                  int height, int windowWidth, int windowHeight);
    void renderPopUp() const;
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

    void writeDescription(float penalty, const std::string& name, int x, int y) const;

    const std::unordered_map<Upgrade, std::string> upgradeDescriptions = {
        { HEALTH, "Aumenta la vida del coche" },
        { ENGINE_FORCE, "Incrementa la aceleracion" },
        { SHIELD, "Proporciona mas vida ante choques" },
        { DAMAGE, "Incrementa el daño al chocar" },
    };

};



#endif //UPGRADE_SCREEN_H
