#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL2pp/Texture.hh>
#include <SDL2pp/Renderer.hh>
#include "../../common_src/constants.h"

class TextureManager {
public:
    explicit TextureManager(SDL2pp::Texture& carsTexture);
    SDL2pp::Rect getCarFrame(CarType type, float angle);

    SDL2pp::Texture& getCarsTexture() const;

private:
    SDL2pp::Texture& carsTexture;
    std::array<CarInfo, CAR_COUNT> carInfos{};
};


#endif //TEXTURE_MANAGER_H
