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
    auto [sx, sy, dx, dy] = camera.getView();


    if (sx + dx > texture.GetWidth()) sx = texture.GetWidth() - dx;
    if (sy + dy > texture.GetHeight()) sy = texture.GetHeight() - dy;

    SDL2pp::Rect srcRect(sx, sy, dx, dy);
    SDL2pp::Rect dstRect( x, y, dx , dy );

    renderer.Copy(texture, srcRect,dstRect);
}