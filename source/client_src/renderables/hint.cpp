#include "hint.h"

Hint::Hint(SDL2pp::Renderer& renderer, TextureManager& tm, const float x,
                       const float y):
        Entity(renderer, tm, x, y), angle(0) {}

void Hint::draw(const Camera& camera) {
    SDL2pp::Texture& tex = tm.getCities().getArrowTexture();

    SDL_Rect srcRect = {0, 0, tex.GetWidth(), tex.GetHeight()};
    SDL_Rect dstRect = {
        static_cast<int>(x),
        static_cast<int>(y),
        tex.GetWidth(),
        tex.GetHeight()
    };

    renderer.Copy(
        tex,
        srcRect,
        dstRect,
        angle,          // Ángulo de rotación
        SDL_Point{tex.GetWidth() / 2, tex.GetHeight() / 2} // Centro de rotación
    );
}


void Hint::setAngle(const float newAngle) {
    angle = fmod(newAngle, 2 * M_PI);
    if (angle < 0) angle += 2 * M_PI;
    angle = angle * 180.0 / M_PI;
}
