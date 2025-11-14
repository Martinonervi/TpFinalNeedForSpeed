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
    SDL2pp::Texture& texture(tm.getCars().getTexture());

    SDL2pp::Rect srcRect(tm.getCars().getFrame(carType, angle));
    const int drawX = static_cast<int>(x - camera.getX() - srcRect.w / 2);
    const int drawY = static_cast<int>(y - camera.getY() - srcRect.h / 2);
    SDL2pp::Rect dstRect(drawX, drawY, srcRect.w/1.25, srcRect.h/1.25);

    renderer.Copy(texture, srcRect,dstRect);

    if (explodedFrame > 0) {
        SDL2pp::Texture& exploTexture(tm.getEffects().getExplosion());
        SDL2pp::Rect srcExploRect(tm.getEffects().getFrame(explodedFrame));

        const int explosionWidth = srcExploRect.w * 3;
        const int explosionHeight = srcExploRect.h * 3;

        const int exploX = static_cast<int>(x - camera.getX() - explosionWidth / 2);
        const int exploY = static_cast<int>(y - camera.getY() - explosionHeight / 2);

        SDL2pp::Rect dstExploRect(exploX, exploY, explosionWidth, explosionHeight);

        renderer.Copy(exploTexture, srcExploRect, dstExploRect);

        explodedFrame = explodedFrame == 8 ? 0 : explodedFrame + 1;
    }
}

void Car::setAngle(const float newAngle) {
    angle = fmod(newAngle, 2 * M_PI);
    if (angle < 0) angle += 2 * M_PI;
}

void Car::setCarType(const CarType newCarType) {
    carType = newCarType;
}

void Car::explode() {
    explodedFrame = 1;
}

float Car::getAngle() const { return angle; }

float Car::getHealthPercentage() const { return 70.0f; }
