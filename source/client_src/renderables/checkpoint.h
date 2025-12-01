#ifndef CHECKPOINT_H
#define CHECKPOINT_H
#include "entity.h"
#include "sdl_drawer.h"


class Checkpoint final: public Entity{
public:
    Checkpoint(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, float x, float y);
    void draw(const Camera& camera) override;
    void setInactive();

private:
    int frame;
    bool active = true;
    SdlDrawer& drawer;

};



#endif //CHECKPOINT_H
