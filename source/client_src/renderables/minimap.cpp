#include "minimap.h"

#include <cmath>
Minimap::Minimap(MapType maptype, SDL2pp::Renderer& renderer, TextureManager& tm,
    std::vector<RecommendedPoint>& pathArray):
        maptype(maptype), renderer(renderer), tm(tm), pathArray(pathArray) {}

void Minimap::draw(const int windowWidth, const int windowHeight,
                   std::unordered_map<ID, std::unique_ptr<Car>>& cars, const ID playerId) const {
    SDL2pp::Texture& texture = tm.getCities().getTexture(MAP_LIBERTY);
    const int miniWidth = 250;   // CONSTANTES
    const int miniHeight = 250;  // CONSTANTES

    const int x = windowWidth - miniWidth - 10;
    const int y = windowHeight;

    SDL2pp::Rect srcRect(0, 0, texture.GetWidth(), texture.GetHeight());
    SDL2pp::Rect dstRect(x, y, miniWidth, miniHeight);

    texture.SetBlendMode(SDL_BLENDMODE_BLEND);
    uint8_t originalAlpha;
    SDL_GetTextureAlphaMod(texture.Get(), &originalAlpha);

    texture.SetAlphaMod(150);

    renderer.Copy(texture, srcRect, dstRect);

    texture.SetAlphaMod(originalAlpha);

    float scaleX = static_cast<float>(miniWidth) / texture.GetWidth();
    float scaleY = static_cast<float>(miniHeight) / texture.GetHeight();

    drawRecommendedPath(x, y, scaleX, scaleY);

    for (const auto& [id, carPtr]: cars) {
        const auto& car = *carPtr;
        int carMapX = car.getX();
        int carMapY = car.getY();

        int miniX = x + static_cast<int>(carMapX * scaleX);
        int miniY = y + static_cast<int>(carMapY * scaleY);
        if (id == playerId) {
            renderer.SetDrawColor(0, 255, 0, 255);
        } else {
            renderer.SetDrawColor(150, 150, 150, 255);
        }
        renderer.FillRect(SDL_Rect{miniX - 3, miniY - 3, 8, 8});
    }
}

void Minimap::drawRecommendedPath(const int x, const int y, float scaleX, float scaleY) const {

    const int thickness = 4;
    renderer.SetDrawColor(0, 0, 0, 255);

    for (size_t i = 1; i < pathArray.size(); i++) {
        int x1 = x + static_cast<int>(pathArray[i - 1].x * scaleX * PIXELS_PER_METER);
        int y1 = y + static_cast<int>(pathArray[i - 1].y * scaleY * PIXELS_PER_METER);
        int x2 = x + static_cast<int>(pathArray[i].x * scaleX * PIXELS_PER_METER);
        int y2 = y + static_cast<int>(pathArray[i].y * scaleY * PIXELS_PER_METER);

        for (int t = -thickness / 2; t <= thickness / 2; t++) {
            renderer.DrawLine(x1 + t, y1, x2 + t, y2);
            renderer.DrawLine(x1, y1 + t, x2, y2 + t);
        }
    }
}

