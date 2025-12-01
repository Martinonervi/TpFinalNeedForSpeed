#include "hint.h"


Hint::Hint(SDL2pp::Renderer& renderer, TextureManager& tm, const float x,
                       const float y):
        Entity(renderer, tm, x, y), angle(0), distance(0) {}

void Hint::draw(const Camera& camera) const {

    int screenW = 1200;
    int screenH = 800;

    if (distance < sqrt(screenW*screenW/9 + screenH*screenH/9)) return;

    SDL2pp::Texture& tex = tm.getCities().getArrowTexture();

    float carXscreen = x - camera.getX();
    float carYscreen = y - camera.getY();

    const float angleDeg = (angle * 180.0 / M_PI) + 90.0;

    constexpr float radius = 90.0f;
    const float drawX = carXscreen + cos(angle) * radius;
    const float drawY = carYscreen + sin(angle) * radius;

    int w = tex.GetWidth() / 2;
    int h = tex.GetHeight() / 2;

    SDL2pp::Rect dstRect = {
        static_cast<int>(drawX - w / 2),
        static_cast<int>(drawY - h / 2),
        w,
        h
    };

    SDL2pp::Point center(w / 2, h / 2);

    renderer.Copy(
        tex,
        SDL2pp::Rect{0, 0, tex.GetWidth(), tex.GetHeight()},
        dstRect,
        angleDeg,
        center
    );

}

void Hint::update(const float newAngle, const float newDistance, const float carX, const float carY) {
    setAngle(newAngle);
    setDistance(newDistance);
    setPosition(carX, carY);
}


void Hint::setAngle(const float newAngle) {
    angle = fmod(newAngle, 2 * M_PI);
    if (angle < 0) angle += 2 * M_PI;
}

void Hint::setDistance(const float newDistance) {
    distance = newDistance;
}
