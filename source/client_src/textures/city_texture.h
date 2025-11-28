#ifndef CITY_TEXTURE_H
#define CITY_TEXTURE_H

#include <SDL2pp/Texture.hh>
#include <map>
#include "../../common_src/constants.h"

class CityTexture {
public:
    CityTexture(SDL2pp::Texture& liberty, SDL2pp::Texture& libertyOver, SDL2pp::Texture& sanAndreas,
        SDL2pp::Texture& vice, SDL2pp::Texture& checkpointTexture, SDL2pp::Texture& arrowTexture,
        SDL2pp::Texture& startBackTexture
        );
    SDL2pp::Texture& getTexture(MapType type) const;
    SDL2pp::Texture& getLibertyOver() const;
    SDL2pp::Texture& getCheckpointTexture() const;
    SDL2pp::Rect getCheckpointFrame(int frame) const;
    SDL2pp::Texture& getArrowTexture() const;
    SDL2pp::Texture& getStartBackTexture() const;

private:
    std::map<MapType, SDL2pp::Texture*> cityTextures;
    SDL2pp::Texture& checkpointTexture;
    SDL2pp::Texture& arrowTexture;
    SDL2pp::Texture& libertyOver;
    SDL2pp::Texture& startBackTexture;
};


#endif //CITY_TEXTURE_H
