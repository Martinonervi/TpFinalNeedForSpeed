#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Renderer.hh>

class Entity {
protected:
    float x, y;
    float angle;
    SDL2pp::Texture* texture;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
public:
    Entity(float x, float y, SDL2pp::Texture* tex);

    virtual ~Entity() {}

    virtual void update(float deltaTime) {}
    virtual void render(SDL2pp::Renderer& renderer, const class Camara& camara);

    float getX() const;
    float getY() const;
};

#endif //ENTITY_H
