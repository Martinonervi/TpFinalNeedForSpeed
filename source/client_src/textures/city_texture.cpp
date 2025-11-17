#include "city_texture.h"

CityTexture::CityTexture(SDL2pp::Texture& liberty,
                                       SDL2pp::Texture& sanAndreas,
                                       SDL2pp::Texture& vice,
                                       SDL2pp::Texture& checkpointTexture) :
checkpointTexture(checkpointTexture)
{
    cityTextures[MAP_LIBERTY] = &liberty;
    cityTextures[MAP_SAN_ANDREAS] = &sanAndreas;
    cityTextures[MAP_VICE] = &vice;
}

SDL2pp::Texture& CityTexture::getTexture(const MapType type) const {
    return *cityTextures.at(type);
}

SDL2pp::Texture& CityTexture::getCheckpointTexture() const {
    return checkpointTexture;
}

SDL2pp::Rect CityTexture::getCheckpointFrame(const int frame) const {
    const int FRAME_WIDTH = 60;
    const int FRAME_HEIGHT = 60;


    const int x = frame / 10 * FRAME_WIDTH;
    constexpr int y = 0;

    return SDL2pp::Rect(x, y, FRAME_WIDTH, FRAME_HEIGHT);
}

