#include "entity.h"

Entity::Entity(
    SDL2pp::Renderer& renderer,
    SDL2pp::Texture& texture,
    const int x, const int y,
    const float angle
    )
    :
renderer(renderer),
texture(texture),
x(x), y(y),
angle(angle)
{}

void Entity::draw() {
    const int w = texture.GetWidth();
    const int h = texture.GetHeight();

    SDL2pp::Rect srcRect( x - w / 2, y - h / 2, w, h ); // Dsps tengo que ver bien que pongo aca
    SDL2pp::Rect dstRect( x - w / 2, y - h / 2, w, h );

    renderer.Copy(texture, srcRect,dstRect);
}

void Entity::setPosition(const int newX, const int newY) {
    x = newX;
    y = newY;
}

int Entity::getX() const { return x; }
int Entity::getY() const { return y; }

