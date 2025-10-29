#include "entity.h"

Entity::Entity(const float x, const float y, SDL2pp::Texture* tex)
        : x(x), y(y), angle(0), texture(tex) {}

void Entity::render(SDL2pp::Renderer& renderer, const Camara& camara) {
    SDL_Rect destiny = { static_cast<int>(x), static_cast<int>(y), dstRect.w, dstRect.h };
    destiny = camara.apply(destiny);
    renderer.Copy(*texture, &srcRect, &destiny, angle);
}

float Entity::getX() const { return x; }
float Entity::getY() const { return y; }