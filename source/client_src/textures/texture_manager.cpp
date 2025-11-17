#include "texture_manager.h"

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/SDL.hh>
#include <SDL_surface.h>

#include "SDL2pp/Surface.hh"

TextureManager::TextureManager(SDL2pp::Renderer& renderer)
{
    flagTexture.emplace(renderer, CHECKPOINT_PATH);
    arrowTexture.emplace(renderer, "../assets/extras/arrow.png");
    barsTexture.emplace(renderer, BARS_PATH);
    upgradeFrameTexture.emplace(renderer, "../assets/extras/item-frame.png");

    hudManager.emplace(*barsTexture, *upgradeFrameTexture);

    speedometerTexture.emplace(loadWithColorKey(renderer, SPEEDOMETER_PATH, 221, 221, 56));
    carsTexture.emplace(loadWithColorKey(renderer, CARS_PATH, 163, 163, 13));
    carManager.emplace(*carsTexture, *speedometerTexture);

    peopleTexture.emplace(loadWithColorKey(renderer, PEOPLE_PATH, 163, 163, 13));
    peopleManager.emplace(*peopleTexture);

    cityLibertyTexture.emplace(renderer, SDL2pp::Surface(LIBERTY_CITY_PATH));
    citySanAndreasTexture.emplace(renderer, SDL2pp::Surface(SAN_ANDREAS_PATH));
    cityViceCityTexture.emplace(renderer, SDL2pp::Surface(VICE_CITY_PATH));
    cityManager.emplace(*cityLibertyTexture, *citySanAndreasTexture, *cityViceCityTexture, *flagTexture, *arrowTexture);

    explosionTextures.emplace(renderer, SDL2pp::Surface(EXPLOSION_PATH));
    effectsManager.emplace(*explosionTextures);
}

SDL2pp::Texture TextureManager::loadWithColorKey(SDL2pp::Renderer& renderer,
                                                 const std::string& path,
                                                 uint8_t r, uint8_t g, uint8_t b) {
    SDL2pp::Surface surface(path);
    const uint32_t key = SDL_MapRGB(surface.Get()->format, r, g, b);
    surface.SetColorKey(true, key);
    SDL2pp::Texture texture(renderer, surface);
    texture.SetBlendMode(SDL_BLENDMODE_BLEND);
    return texture;
}

CarTexture& TextureManager::getCars() { return carManager.value(); }
PeopleTexture& TextureManager::getPeople() { return peopleManager.value(); }
CityTexture& TextureManager::getCities() { return cityManager.value(); }
EffectsTexture& TextureManager::getEffects() { return effectsManager.value(); }
HudTexture& TextureManager::getHud() { return hudManager.value(); }
