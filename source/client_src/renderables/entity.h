#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

class Entity {
public:
    Entity(SDL2pp::Renderer& renderer, SDL2pp::Texture& texture, int x, int y, float angle);
    virtual ~Entity() = default;

    virtual void draw();
    virtual void setPosition(int newX, int newY);

    int getX() const;
    int getY() const;

protected:
    SDL2pp::Renderer& renderer;
    SDL2pp::Texture& texture;

    int x;
    int y;
};

#endif //ENTITY_H


