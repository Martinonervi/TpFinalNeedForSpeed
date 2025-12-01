#ifndef CAR_H
#define CAR_H

#include "entity.h"

class Car final : public Entity {
public:
    Car(
        SDL2pp::Renderer& renderer,
        TextureManager& tm,
        float startX, float startY,
        CarType carType,
        float angle
        );

    void update(float newX, float newY, float newAngle);
    void draw(const Camera& camera) override;
    void drawExplosion(const Camera& camera);
    void setMaxHealth(float newMaxHealth);

    void setAngle(float newAngle);
    void setCarType(CarType newCarType);
    void setHealth(float health);

    CarState getState() const;
    float getAngle() const;
    float getHealthPercentage() const;
    float getSpeed() const;
    void setSpeed(float newSpeed);
    void addUpgrade(Upgrade newUp);
    std::vector<Upgrade> getUpgrades();
    float getHealth() const;
    float getMaxHealth() const;
    void clearUpgrades();
    bool getInCamera() const;

private:
    float health = 100.0f;
    float maxHealth = 100.0f;
    float speed;
    float angle;
    CarType carType;
    CarState carState;
    int explosionFrame = 0;
    int frameTicks = 0;
    static constexpr int TICKS_PER_FRAME = 4;
    std::vector<Upgrade> upgrades;
    bool inCamera = true;

    void setInCamera(bool newInCamer);
};

#endif //CAR_H
