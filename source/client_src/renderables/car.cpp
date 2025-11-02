#include "car.h"

Car::Car(
    SDL2pp::Renderer& renderer,
    TextureManager& tm,
    const float startX, const float startY,
    const CarType carType,
    const float angle
    )
    : Entity(renderer, tm, startX, startY), angle(angle), carType(carType)
{}

void Car::update(const float newX, const float newY, const float newAngle) {
    setPosition((newX), (newY));
    setAngle(newAngle);
}

void Car::draw(const Camera& camera) {
    SDL2pp::Texture& texture(tm.getCarsTexture());

    SDL2pp::Rect srcRect(tm.getCarFrame(carType, angle));
    SDL2pp::Rect dstRect(x - camera.getX(), y - camera.getY(), srcRect.w, srcRect.h);

    renderer.Copy(texture, srcRect,dstRect);
}

void Car::setAngle(const float newAngle) {
    angle = fmod(newAngle, 2 * M_PI);
    if (angle < 0) angle += 2 * M_PI;
}

void Car::setCarType(const CarType newCarType) {
    carType = newCarType;
}

float Car::getAngle() const { return angle; }