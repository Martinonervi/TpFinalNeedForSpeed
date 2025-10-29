#ifndef AUTO_H
#define AUTO_H
#include "entity.h"

// Todos los Auto van a ser Car
class Auto : public Entity {
public:
    Car(float x, float y, SDL2pp::Texture* tex);
    ~Car();

    void update(float deltaTime) override;
    void accelerate(const float v) { speed = v; }
    void steer(float deltaAngle);

private:
    float speed;
};



#endif //AUTO_H
