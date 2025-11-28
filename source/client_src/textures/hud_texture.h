#ifndef HUD_TEXTURE_H
#define HUD_TEXTURE_H
#include <SDL2pp/Texture.hh>

#include "../../common_src/opcodes.h"

class HudTexture {
public:
    HudTexture(SDL2pp::Texture& healthAndNitro, SDL2pp::Texture& swordAndShield, SDL2pp::Texture& upgradeFrame,
        SDL2pp::Texture& upgrades);
    SDL2pp::Rect getHealthBar() const;
    SDL2pp::Rect getEnergyBar() const;
    SDL2pp::Texture& getBarsTexture() const;
    SDL2pp::Texture& getUpgradeFrame() const;
    SDL2pp::Texture& getUpgrades() const;
    SDL2pp::Rect getUpgradeIconRect(Upgrade type) const;

private:
    SDL2pp::Texture& healthAndNitro;
    SDL2pp::Texture& swordAndShield;
    SDL2pp::Texture& upgradeFrame;
    SDL2pp::Texture& upgrades;

    const std::unordered_map<Upgrade, SDL2pp::Rect> rects = {
        { HEALTH, SDL2pp::Rect(1, 1, 14, 17) },
        { ENGINE_FORCE, SDL2pp::Rect(29, 1, 17, 17) },
        { SHIELD, SDL2pp::Rect(63, 1, 16, 17) },
        { DAMAGE,  SDL2pp::Rect(95, 1, 16, 17) },
    };
};



#endif //HUD_TEXTURES_H
