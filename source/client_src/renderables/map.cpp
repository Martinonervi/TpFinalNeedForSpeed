#include "map.h"

#include "../client_camera.h"

Map::Map(
    SDL2pp::Renderer& renderer,
    TextureManager& tm
    //const MapType mapType Hay que implementarlo (path???)
    )
    : Entity(renderer, tm, 0, 0) //, mapType(mapType)
{}


void Map::draw(const Camera& camera) const {
    SDL2pp::Texture& texture(tm.getMapsTexture());

    SDL2pp::Rect srcRect(camera.getView());
    SDL2pp::Rect dstRect( x, y, srcRect.w, srcRect.h );

    renderer.Copy(texture, srcRect,dstRect);
}