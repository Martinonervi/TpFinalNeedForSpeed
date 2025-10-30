#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

#include "../textures/texture_manager.h"

class Entity {
public:
    Entity(SDL2pp::Renderer& renderer, TextureManager& tm, int x, int y);
    virtual ~Entity() = default;

    virtual void draw(int dx, int dy);
    virtual void setPosition(int newX, int newY);

    int getX() const;
    int getY() const;

protected:
    SDL2pp::Renderer& renderer;
    TextureManager& tm;

    int x;
    int y;
};

#endif //ENTITY_H


