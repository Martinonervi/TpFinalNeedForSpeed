#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

#include "../textures/texture_manager.h"
#include "../../common_src/constants.h"

class Entity {
public:
    Entity(SDL2pp::Renderer& renderer, TextureManager& tm, float x, float y);
    virtual ~Entity() = default;

    virtual void draw();
    void setPosition(float newX, float newY);
    void setLayer(EntityLayer l);

    EntityLayer getLayer() const;
    float getX() const;
    float getY() const;

protected:
    SDL2pp::Renderer& renderer;
    TextureManager& tm;
    EntityLayer layer;

    float x;
    float y;
};

#endif //ENTITY_H


