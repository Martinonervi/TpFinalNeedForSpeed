#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL2pp/Renderer.hh>

#include "car_texture.h"
#include "city_texture.h"
#include "effects_texture.h"
#include "hud_texture.h"

class TextureManager {
public:
    explicit TextureManager(SDL2pp::Renderer& renderer);

    CarTexture& getCars();
    CityTexture& getCities();
    EffectsTexture& getEffects();
    HudTexture& getHud();

private:
    std::optional<SDL2pp::Texture> carsTexture;
    std::optional<SDL2pp::Texture> speedometerTexture;
    std::optional<SDL2pp::Texture> cityLibertyTexture;
    std::optional<SDL2pp::Texture> cityLibertyOverTexture;
    std::optional<SDL2pp::Texture> citySanAndreasTexture;
    std::optional<SDL2pp::Texture> cityViceCityTexture;
    std::optional<SDL2pp::Texture> explosionTextures;
    std::optional<SDL2pp::Texture> healthNitroTexture;
    std::optional<SDL2pp::Texture> swordShieldTexture;
    std::optional<SDL2pp::Texture> flagTexture;
    std::optional<SDL2pp::Texture> arrowTexture;
    std::optional<SDL2pp::Texture> upgradeFrameTexture;
    std::optional<SDL2pp::Texture> upgradeTexture;
    std::optional<SDL2pp::Texture> startBackTexture;

    std::optional<CarTexture> carManager;
    std::optional<CityTexture> cityManager;
    std::optional<EffectsTexture> effectsManager;
    std::optional<HudTexture> hudManager;

    static SDL2pp::Texture loadWithColorKey(
        SDL2pp::Renderer& renderer,
        const std::string& path,
        uint8_t r, uint8_t g, uint8_t b
        );
};


#endif //TEXTURE_MANAGER_H
