#include "city_texture.h"

CityTexture::CityTexture(SDL2pp::Texture& liberty,
                    SDL2pp::Texture& libertyOver,
                    SDL2pp::Texture& sanAndreas,
                    SDL2pp::Texture& vice,
                    SDL2pp::Texture& checkpointTexture,
                    SDL2pp::Texture& arrowTexture,
                    SDL2pp::Texture& startBackTexture
                    ) :
checkpointTexture(checkpointTexture), arrowTexture(arrowTexture), libertyOver(libertyOver),
startBackTexture(startBackTexture)
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

SDL2pp::Texture& CityTexture::getArrowTexture() const {
    return arrowTexture;
}

SDL2pp::Texture& CityTexture::getLibertyOver() const {
    return libertyOver;
}

SDL2pp::Texture& CityTexture::getStartBackTexture() const {
    return startBackTexture;
}

