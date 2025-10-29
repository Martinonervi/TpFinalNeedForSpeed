#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL2pp/Texture.hh>
#include <SDL2pp/Renderer.hh>
#include <string>
#include "../../common_src/constants.h"

class TextureManager {
public:
    TextureManager(SDL2pp::Renderer& renderer);

    void loadCarsTexture(const std::string& path);

    SDL2pp::Texture& getCarsTexture() { return carsTexture; }

    // Devuelve el rect según tipo y ángulo
    SDL2pp::Rect getCarFrame(CarType type, float angle);

private:
    SDL2pp::Renderer& renderer;
    SDL2pp::Texture carsTexture;

    // Información interna de cada auto
    std::array<CarInfo, CAR_COUNT> carInfos;
};


#endif //TEXTURE_MANAGER_H
