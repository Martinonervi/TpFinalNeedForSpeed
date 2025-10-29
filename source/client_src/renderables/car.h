#ifndef CAR_H
#define CAR_H

#include "entity.h"

class Car final : public Entity {
public:
    Car(
        SDL2pp::Renderer& renderer,
        TextureManager& tm,
        int startX, int startY,
        CarType carType,
        float angle
        );

    void update(float newX, float newY, float newAngle);
    void draw(int dx, int dy) override;

    void setAngle(float newAngle);
    void setCarType(CarType newCarType);
    float getAngle() const;

private:
    float angle;
    CarType carType;
};

#endif //CAR_H
