#include "map.h"

#include "../client_camera.h"

Map::Map(
    SDL2pp::Renderer& renderer,
    TextureManager& tm,
    const MapType mapType
    )
    : Entity(renderer, tm, 0, 0), mapType(mapType)
{}


void Map::draw(const Camera& camera) {
    SDL2pp::Texture& texture(tm.getCities().getTexture(mapType));

    SDL2pp::Rect srcRect(camera.getView());
    SDL2pp::Rect dstRect( x, y, srcRect.w*1.5, srcRect.h*1.5 ); // Constantes

    renderer.Copy(texture, srcRect,dstRect);
}