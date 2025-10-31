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

void Car::draw() {
    SDL2pp::Texture& texture(tm.getCarsTexture());
    const int w = texture.GetWidth();
    const int h = texture.GetHeight();

    SDL2pp::Rect srcRect(tm.getCarFrame(carType, angle));
    SDL2pp::Rect dstRect( x, y, w, h ); // Tengo que ver como manejar esto

    renderer.Copy(texture, srcRect,dstRect);
}

void Car::setAngle(const float newAngle) {
    angle = newAngle;
}

void Car::setCarType(const CarType newCarType) {
    carType = newCarType;
}

float Car::getAngle() const { return angle; }