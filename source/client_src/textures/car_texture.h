#ifndef CAR_TEXTURE_H
#define CAR_TEXTURE_H

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

#include "../../common_src/constants.h"
#include <map>

class CarTexture {
public:
    CarTexture(SDL2pp::Texture& carsTexture, SDL2pp::Texture& speedometerTexture);
    SDL2pp::Rect getFrame(CarType type, float angle) const;
    SDL2pp::Texture& getTexture() const;
    SDL2pp::Texture& getSpeedometer() const;
    SDL2pp::Rect getNeedleFrame(float speed) const;
    SDL2pp::Rect getDialFrame() const;

private:
    SDL2pp::Texture& carsTexture;
    SDL2pp::Texture& speedometerTexture;
    std::map<CarType, CarInfo> carInfo;
};



#endif //CAR_TEXTURE_H
