#ifndef CITY_TEXTURE_H
#define CITY_TEXTURE_H

#include <SDL2pp/Texture.hh>
#include <map>
#include "../../common_src/constants.h"

class CityTexture {
public:
    CityTexture(SDL2pp::Texture& liberty, SDL2pp::Texture& sanAndreas, SDL2pp::Texture& vice);
    SDL2pp::Texture& getTexture(MapType type) const;

private:
    std::map<MapType, SDL2pp::Texture*> cityTextures;
};


#endif //CITY_TEXTURE_H
