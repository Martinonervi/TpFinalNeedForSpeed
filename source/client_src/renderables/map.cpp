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
    float zoom = 1.5f;

    SDL2pp::Texture& texture(tm.getCities().getTexture(mapType));
    auto [sx, sy, dx, dy] = camera.getView();

    int srcW = static_cast<int>(dx / zoom);
    int srcH = static_cast<int>(dy / zoom);

    if (sx + srcW > texture.GetWidth()) sx = texture.GetWidth() - srcW;
    if (sy + srcH > texture.GetHeight()) sy = texture.GetHeight() - srcH;

    SDL2pp::Rect srcRect(sx, sy, srcW, srcH);
    SDL2pp::Rect dstRect( x, y, dx , dy );

    renderer.Copy(texture, srcRect,dstRect);
}