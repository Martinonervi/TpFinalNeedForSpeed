#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL2pp/Renderer.hh>

#include "car_texture.h"
#include "city_texture.h"
#include "effects_texture.h"
#include "people_texture.h"

class TextureManager {
public:
    explicit TextureManager(SDL2pp::Renderer& renderer);

    CarTexture& getCars();
    PeopleTexture& getPeople();
    CityTexture& getCities();
    EffectsTexture& getEffects();

private:
    std::optional<SDL2pp::Texture> carsTexture;
    std::optional<SDL2pp::Texture> speedometerTexture;
    std::optional<SDL2pp::Texture> peopleTexture;
    std::optional<SDL2pp::Texture> cityLibertyTexture;
    std::optional<SDL2pp::Texture> citySanAndreasTexture;
    std::optional<SDL2pp::Texture> cityViceCityTexture;
    std::optional<SDL2pp::Texture> explosionTextures;

    std::optional<CarTexture> carManager;
    std::optional<PeopleTexture> peopleManager;
    std::optional<CityTexture> cityManager;
    std::optional<EffectsTexture> effectsManager;

    static SDL2pp::Texture loadWithColorKey(
        SDL2pp::Renderer& renderer,
        const std::string& path,
        uint8_t r, uint8_t g, uint8_t b
        );
};


#endif //TEXTURE_MANAGER_H
