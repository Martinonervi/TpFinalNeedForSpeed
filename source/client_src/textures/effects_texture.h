#ifndef EFFECTS_TEXTURE_H
#define EFFECTS_TEXTURE_H

#include <SDL2pp/Texture.hh>
#include <map>
#include "../../common_src/constants.h"

class EffectsTexture {
public:
    explicit EffectsTexture(SDL2pp::Texture& effectsTexture);
    SDL2pp::Texture& getExplosion() const;
    SDL2pp::Rect getFrame(int frameIndex) const;
private:
    SDL2pp::Texture& explosionTexture;
    int frameWidth;
    int frameHeight;
    int totalFrames;

};



#endif //EFFECTS_TEXTURE_H
