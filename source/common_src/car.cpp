#include "car.h"

Car::Car(SDL2pp::Texture& tex,
         SDL2pp::Renderer& rend,
         const int x, const int y,
         const int w, const int h,
         const int screenX, const int screenY,
         const int spd)
    : texture(tex), renderer(rend), speed(spd)
{
    srcRect = {x, y, w, h};
    dstRect = {screenX, screenY, w, h};
}


void Car::draw() const {
    SDL2pp::Rect src(srcRect.x, srcRect.y, srcRect.w, srcRect.h);
    SDL2pp::Rect dst(dstRect.x, dstRect.y, dstRect.w, dstRect.h);

    renderer.Copy(texture, src, dst);
}

// Cada clic de la tecla sera un move con cierta velocidad.
void Car::move(const int dx, const int dy) {
    dstRect.x += dx * speed;
    dstRect.y += dy * speed;
}

int Car::getX() const { return dstRect.x; }
int Car::getY() const { return dstRect.y; }

void Car::setPosition(int x, int y) {
    dstRect.x = x;
    dstRect.y = y;
}

void Car::setSpeed(const int spd) { speed = spd; }
int Car::getSpeed() const { return speed; }
