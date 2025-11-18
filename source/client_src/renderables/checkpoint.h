#ifndef CHECKPOINT_H
#define CHECKPOINT_H
#include "entity.h"


class Checkpoint final: public Entity{
public:
    Checkpoint(SDL2pp::Renderer& renderer, TextureManager& tm, float x, float y);
    void draw(const Camera& camera) override;
    void setInactive();

private:
    int frame;
    bool active = true;

};



#endif //CHECKPOINT_H
