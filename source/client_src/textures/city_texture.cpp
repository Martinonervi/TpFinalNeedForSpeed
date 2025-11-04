#include "city_texture.h"

CityTexture::CityTexture(SDL2pp::Texture& liberty,
                                       SDL2pp::Texture& sanAndreas,
                                       SDL2pp::Texture& vice) {
    cityTextures[MAP_LIBERTY] = &liberty;
    cityTextures[MAP_SAN_ANDREAS] = &sanAndreas;
    cityTextures[MAP_VICE] = &vice;
}

SDL2pp::Texture& CityTexture::getTexture(MapType type) {
    return *cityTextures.at(type);
}
