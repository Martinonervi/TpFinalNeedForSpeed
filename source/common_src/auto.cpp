#include "auto.h"

// Todos los Auto van a ser Car

Auto::Car(
    const float x,
    const float y,
    SDL2pp::Texture* tex
    )
    : Entity(x, y, tex), speed(0)
{
    srcRect = {0, 0, 64, 64};
    dstRect = {0, 0, 64, 64};
}

void Auto::update(const float deltaTime) {
    x += speed * deltaTime * cos(angle * M_PI / 180);
    y += speed * deltaTime * sin(angle * M_PI / 180);
}
void Auto::steer(const float deltaAngle) { speed += deltaAngle; }