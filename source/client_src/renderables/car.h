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
    void explode();

    void setAngle(float newAngle);
    void setCarType(CarType newCarType);
    float getAngle() const;
    float getHealthPercentage() const;

private:
    float angle;
    CarType carType;
    int explodedFrame = 0;
};

#endif //CAR_H
