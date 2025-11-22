#include "hud_texture.h"

HudTexture::HudTexture(SDL2pp::Texture& healthAndNitro, SDL2pp::Texture& swordAndShield, SDL2pp::Texture& upgradeFrame,
        SDL2pp::Texture& upgrades) :
    healthAndNitro(healthAndNitro), swordAndShield(swordAndShield), upgradeFrame(upgradeFrame), upgrades(upgrades)
{}

SDL2pp::Rect HudTexture::getHealthBar() const {
    int x = 2;
    int y = 2;
    int w = 60;
    int h = 11;
    return {x, y, w, h};
}

SDL2pp::Rect HudTexture::getEnergyBar() const {
    int x = 3;
    int y = 16;
    int w = 60;
    int h = 16;
    return {x, y, w, h};
}

SDL2pp::Texture& HudTexture::getBarsTexture() const { // cambiar nombre
    return healthAndNitro;
}

SDL2pp::Texture& HudTexture::getUpgradeFrame() const {
    return upgradeFrame;
}

SDL2pp::Texture& HudTexture::getUpgrades() const {
    return upgrades;
}
