#include "minimap.h"

#include <cmath>
std::vector<SDL_Point> recommendedPath = {
        {20, 20},   {40, 30},  {70, 50},  {100, 90}, {130, 140}, {160, 180}, {200, 200}, {220, 170},
        {230, 130}, {210, 90}, {180, 60}, {140, 40}, {100, 30},  {60, 25},   {30, 20}};


Minimap::Minimap(MapType maptype, SDL2pp::Renderer& renderer, TextureManager& tm):
        maptype(maptype), renderer(renderer), tm(tm) {}

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

    texture.SetAlphaMod(115);

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

    const int thickness = 3;
    renderer.SetDrawColor(0, 0, 0, 255);

    for (size_t i = 1; i < recommendedPath.size(); i++) {
        int x1 = x + static_cast<int>(recommendedPath[i - 1].x * 1);
        int y1 = y + static_cast<int>(recommendedPath[i - 1].y * 1);
        int x2 = x + static_cast<int>(recommendedPath[i].x * 1);
        int y2 = y + static_cast<int>(recommendedPath[i].y * 1);

        for (int t = -thickness / 2; t <= thickness / 2; t++) {
            renderer.DrawLine(x1 + t, y1, x2 + t, y2);  // grosor horizontal
            renderer.DrawLine(x1, y1 + t, x2, y2 + t);  // grosor vertical
        }
    }
}

