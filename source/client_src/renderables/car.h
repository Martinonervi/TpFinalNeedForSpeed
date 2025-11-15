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

    void setAngle(float newAngle);
    void setCarType(CarType newCarType);
    void setHealth(float health);

    CarState getState() const;
    float getAngle() const;
    float getHealth() const;

private:
    float health = 100.0f;
    float angle;
    CarType carType;
    CarState carState;
    int explosionFrame = 0;
    int frameTicks = 0;         // cuántos ticks pasó en el mismo frame
    static constexpr int TICKS_PER_FRAME = 4;   // ajustá a gusto (2–6 suele quedar bien)


};

#endif //CAR_H
