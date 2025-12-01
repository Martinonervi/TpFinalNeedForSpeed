#include "car.h"

Car::Car(
    SDL2pp::Renderer& renderer,
    TextureManager& tm,
    const float startX, const float startY,
    const CarType carType,
    const float angle
    )
    : Entity(renderer, tm, startX, startY), angle(angle), carType(carType), carState(ALIVE)
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
}

void Car::setAngle(const float newAngle) {
    angle = fmod(newAngle, 2 * M_PI);
    if (angle < 0) angle += 2 * M_PI;
}

void Car::setCarType(const CarType newCarType) {
    carType = newCarType;
}

void Car::setHealth(const float newHealth) {
    if (newHealth <= 0 && (carState == LOW_HEALTH || carState == ALIVE)) {
        explosionFrame = 1;
        carState = EXPLODING;
    } else if (newHealth <= 20 && carState == ALIVE) {
        carState = LOW_HEALTH;
    }
    health = newHealth;
}

void Car::drawExplosion(const Camera& camera) {
    SDL2pp::Texture& tex = tm.getEffects().getExplosion();

    SDL2pp::Rect src(tm.getEffects().getFrame(explosionFrame));

    const int size = src.w * 3;
    const int drawX = int(x - camera.getX() - size / 2);
    const int drawY = int(y - camera.getY() - size / 2);

    SDL2pp::Rect dst(drawX, drawY, size, size);
    renderer.Copy(tex, src, dst);

    frameTicks++;
    if (frameTicks >= TICKS_PER_FRAME) {
        frameTicks = 0;
        explosionFrame++;
    }

    if (explosionFrame >= 7) {
        carState = DESTROYED;
    }
}

void Car::setMaxHealth(const float newMaxHealth) {
    maxHealth = newMaxHealth;
}

CarState Car::getState() const { return carState; }

float Car::getAngle() const { return angle; }

float Car::getHealthPercentage() const { return health/maxHealth; }

float Car::getSpeed() const { return speed; }
void Car::setSpeed(const float newSpeed) { speed=newSpeed; }

void Car::addUpgrade(const Upgrade newUp) {
    if (upgrades.size() == 3) return;
    upgrades.push_back(newUp);
}

std::vector<Upgrade> Car::getUpgrades() {
    return upgrades;
}

void Car::clearUpgrades() {
    upgrades.clear();
}

float Car::getHealth() const { return health; }

float Car::getMaxHealth() const { return maxHealth; }