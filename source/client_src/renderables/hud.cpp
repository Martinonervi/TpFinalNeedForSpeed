#include "hud.h"

Hud::Hud(SDL2pp::Renderer& renderer, TextureManager& tm, MapType maptype): map(maptype, renderer, tm), renderer(renderer) {}

void Hud::drawOverlay(int mapX, int mapY, std::unordered_map<ID, std::unique_ptr<Car>>& cars, ID playerId) const {
    map.draw(mapX, 10, cars);
    auto it = cars.find(playerId);
    if (it == cars.end() || !it->second) return;
    const Car& playerCar = *(it->second);
    float healthPercent = playerCar.getHealthPercentage();
    drawHealthBar(renderer, mapY, healthPercent);
}

void Hud::drawHealthBar(SDL2pp::Renderer& renderer, int windowHeight, float healthPercent) const {
    if (healthPercent < 0.0f) healthPercent = 0.0f;
    if (healthPercent > 100.0f) healthPercent = 100.0f;

    int barWidth = 200;
    int barHeight = 20;
    int x = 20;
    int y = windowHeight - barHeight - 20;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

    renderer.SetDrawColor(50, 50, 50, 200);
    renderer.FillRect(SDL_Rect{x - 2, y - 2, barWidth + 4, barHeight + 4});

    int filledWidth = static_cast<int>((healthPercent / 100.0f) * barWidth);
    renderer.SetDrawColor(200, 0, 0, 255);
    renderer.FillRect(SDL_Rect{x, y, filledWidth, barHeight});
}

