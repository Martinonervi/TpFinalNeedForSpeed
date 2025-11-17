#ifndef HINT_H
#define HINT_H

#include "entity.h"



class Hint : public Entity {
public:
    Hint(SDL2pp::Renderer& renderer, TextureManager& tm, float x, float y);
    void draw(const Camera& camera) override;
    void setAngle(float newAngle);

private:
    float angle;
};



#endif //HINT_H
