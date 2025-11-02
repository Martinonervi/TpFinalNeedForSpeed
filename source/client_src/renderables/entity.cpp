#include "entity.h"

#include "../textures/texture_manager.h"

Entity::Entity(
    SDL2pp::Renderer& renderer,
    TextureManager& tm,
    const float x, const float y
    )
    :
renderer(renderer),
tm(tm),
x(x), y(y)
{}

void Entity::setPosition(const float newX, const float newY) {
    x = newX;
    y = newY;
}

void Entity::draw(const Camera& camera) {
    // Por defecto, no hace nada
}

void Entity::setLayer(const EntityLayer l) { layer = l; }
EntityLayer Entity::getLayer() const { return layer; }

float Entity::getX() const { return x; }
float Entity::getY() const { return y; }

