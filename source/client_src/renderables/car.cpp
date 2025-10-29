#include "car.h"

Car::Car(
    SDL2pp::Renderer& renderer,
    SDL2pp::Texture& texture,
    const int spriteX, const int spriteY,
    const int spriteW, const int spriteH,
    const int startX, const int startY,
    const float angle
    )
    : Entity(renderer, texture, startX, startY, angle)
{
    srcRect = SDL2pp::Rect(spriteX, spriteY, spriteW, spriteH); // Se puede manejar de otra manera
}

void Car::update(const float newX, const float newY, const float newAngle) {
    setPosition(static_cast<int>(newX), static_cast<int>(newY));
    setAngle(newAngle);
}

void Car::setAngle(const float newAngle) {
    angle = newAngle;
}

float Car::getAngle() const { return angle; }