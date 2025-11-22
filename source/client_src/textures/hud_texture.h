#ifndef HUD_TEXTURE_H
#define HUD_TEXTURE_H
#include <SDL2pp/Texture.hh>

class HudTexture {
public:
    HudTexture(SDL2pp::Texture& healthAndNitro, SDL2pp::Texture& swordAndShield, SDL2pp::Texture& upgradeFrame,
        SDL2pp::Texture& upgrades);
    SDL2pp::Rect getHealthBar() const;
    SDL2pp::Rect getEnergyBar() const;
    SDL2pp::Texture& getBarsTexture() const;
    SDL2pp::Texture& getUpgradeFrame() const;
    SDL2pp::Texture& getUpgrades() const;

private:
    SDL2pp::Texture& healthAndNitro;
    SDL2pp::Texture& swordAndShield;
    SDL2pp::Texture& upgradeFrame;
    SDL2pp::Texture& upgrades;

};



#endif //HUD_TEXTURES_H
