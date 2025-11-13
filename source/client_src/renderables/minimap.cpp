#include "minimap.h"
#include <cmath>

Minimap::Minimap(MapType maptype, SDL2pp::Renderer& renderer, TextureManager& tm):
maptype(maptype), renderer(renderer), tm(tm) {}

void Minimap::draw(const int windowWidth, const int windowHeight, std::unordered_map<ID, std::unique_ptr<Car>>& cars) const {
    SDL2pp::Texture& texture = tm.getCities().getTexture(MAP_LIBERTY);

    int miniWidth = 250;
    int miniHeight = 250;

    int x = windowWidth - miniWidth - 10;
    int y = windowHeight;

    SDL2pp::Rect srcRect(0, 0, texture.GetWidth(), texture.GetHeight());
    SDL2pp::Rect dstRect(x, y, miniWidth, miniHeight);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

    renderer.SetDrawColor(30, 30, 30, 230);
    renderer.FillRect(SDL_Rect{x - 5, y - 5, miniWidth + 10, miniHeight + 10});

    renderer.SetDrawColor(180, 180, 180, 150);
    renderer.FillRect(SDL_Rect{x - 2, y - 2, miniWidth + 4, miniHeight + 4});

    renderer.Copy(texture, srcRect, dstRect);

    float scaleX = static_cast<float>(miniWidth) / texture.GetWidth();
    float scaleY = static_cast<float>(miniHeight) / texture.GetHeight();

    for (const auto& [id, carPtr] : cars) {
        const auto& car = *carPtr;
        int carMapX = car.getX();
        int carMapY = car.getY();

        int miniX = x + static_cast<int>(carMapX * scaleX);
        int miniY = y + static_cast<int>(carMapY * scaleY);

        renderer.SetDrawColor((id * 70) % 255, (id * 130) % 255, (id * 200) % 255, 255);
        renderer.FillRect(SDL_Rect{miniX - 3, miniY - 3, 8, 8});
    }
}
