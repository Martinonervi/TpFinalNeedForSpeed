#include "effects_texture.h"

EffectsTexture::EffectsTexture(SDL2pp::Texture& explosion) : explosionTexture(explosion)
{
    frameWidth = 32;
    frameHeight = 32;
    totalFrames = 8;
}

SDL2pp::Rect EffectsTexture::getFrame(int frameIndex) const {
    frameIndex = frameIndex % totalFrames;

    // Como es una sola fila de frames
    int x = frameIndex * frameWidth;
    int y = 0;

    return {x, y, frameWidth, frameHeight};
}

SDL2pp::Texture& EffectsTexture::getExplosion() const {
    return explosionTexture;
}
