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

void Entity::draw() {
    /*
    const int w = texture.GetWidth();
    const int h = texture.GetHeight();

    SDL2pp::Rect srcRect( dx, y - h / 2, w, h ); // Dsps tengo que ver bien que pongo aca
    SDL2pp::Rect dstRect( dx, dy, w, h );

    renderer.Copy(texture, srcRect,dstRect);
*/
}

void Entity::setPosition(const float newX, const float newY) {
    x = newX;
    y = newY;
}

void Entity::setLayer(const EntityLayer l) { layer = l; }
EntityLayer Entity::getLayer() const { return layer; }

float Entity::getX() const { return x; }
float Entity::getY() const { return y; }

