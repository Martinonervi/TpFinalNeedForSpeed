#include "car.h"

Car::Car(
    SDL2pp::Texture& tex,
    SDL2pp::Renderer& rend,
    const int carIndex, const int screenX,
    const int screenY, const int spd
    )
    : texture(tex), renderer(rend), speed(spd)
{
    sprite = &CAR_SPRITES[carIndex];
    const auto& frame = sprite->frames[0];
    dstRect = {screenX, screenY, frame.width, frame.height};
}

void Car::update(const int dx, const int dy) {
    if (dx == 1) {
        currentFrame = 0;
    } else if (dx == -1) {
        currentFrame = 8;
    } else if (dy == 1) {
        currentFrame = 4;
    } else {
        currentFrame = 12;
    }
}

void Car::draw() const {
    const auto& frame = sprite->frames[currentFrame];
    SDL2pp::Rect src(frame.x, frame.y, frame.width, frame.height);
    SDL2pp::Rect dst(dstRect.x, dstRect.y, frame.width, frame.height);
    renderer.Copy(texture, src, dst);
}

void Car::move(const int dx, const int dy) {
    update(dx, dy);
    dstRect.x += dx * speed;
    dstRect.y += dy * speed;

    if (dy > 0) direction = 0;
    else if (dy < 0) direction = 1;
}
