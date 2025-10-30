#include "entity.h"

#include "../textures/texture_manager.h"

Entity::Entity(
    SDL2pp::Renderer& renderer,
    TextureManager& tm,
    const int x, const int y
    )
    :
renderer(renderer),
tm(tm),
x(x), y(y)
{}

void Entity::draw(const int dx, const int dy) {
    /*
    const int w = texture.GetWidth();
    const int h = texture.GetHeight();

    SDL2pp::Rect srcRect( dx, y - h / 2, w, h ); // Dsps tengo que ver bien que pongo aca
    SDL2pp::Rect dstRect( dx, dy, w, h );

    renderer.Copy(texture, srcRect,dstRect);
*/
}

void Entity::setPosition(const int newX, const int newY) {
    x = newX;
    y = newY;
}

int Entity::getX() const { return x; }
int Entity::getY() const { return y; }

