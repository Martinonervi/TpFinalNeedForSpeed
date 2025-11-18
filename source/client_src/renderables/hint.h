#ifndef HINT_H
#define HINT_H

#include "entity.h"



class Hint : public Entity {
public:
    Hint(SDL2pp::Renderer& renderer, TextureManager& tm, float x, float y);
    void draw(const Camera& camera) const;
    void update(float newAngle, float newDistance, float carX, float carY);
    void setAngle(float newAngle);
    void setDistance(float newDistance);

private:
    float angle;
    float distance;
};



#endif //HINT_H
