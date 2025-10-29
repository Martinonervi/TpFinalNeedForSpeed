#ifndef CAR_H
#define CAR_H

#include "entity.h"

class Car final : public Entity {
public:
    Car(
        SDL2pp::Renderer& renderer,
        SDL2pp::Texture& texture,
        int spriteX, int spriteY,
        int spriteW, int spriteH,
        int startX, int startY
        );

    void update(float newX, float newY, float newAngle);

    void setAngle(float newAngle);
    float getAngle() const;

private:
    SDL2pp::Rect srcRect;
    float angle;
};

#endif //CAR_H
