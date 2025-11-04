#ifndef CAR_TEXTURE_H
#define CAR_TEXTURE_H

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

#include "../../common_src/constants.h"
#include <map>

class CarTexture {
public:
    explicit CarTexture(SDL2pp::Texture& carsTexture);
    SDL2pp::Rect getFrame(CarType type, float angle) const;
    SDL2pp::Texture& getTexture() const;

private:
    SDL2pp::Texture& carsTexture;
    std::map<CarType, CarInfo> carInfo;

};



#endif //CAR_TEXTURE_H
