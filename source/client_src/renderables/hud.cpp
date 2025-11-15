#include "hud.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Hud::Hud(SDL2pp::Renderer& renderer, TextureManager& tm, MapType maptype)
    : map(maptype, renderer, tm), renderer(renderer), tm(tm)
{
    loadFont();
}


void Hud::drawOverlay(int x, int y, std::unordered_map<ID, std::unique_ptr<Car>>& cars, ID playerId) {
    map.draw(x, 10, cars, playerId);
    const auto it = cars.find(playerId);
    if (it == cars.end() || !it->second) return;
    const Car& playerCar = *it->second;

    drawHealthBar(renderer, y, playerCar.getHealthPercentage());
    drawSpeed(renderer, 150.0f, x, y);
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

void Hud::loadFont() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    }

    font = TTF_OpenFont("../assets/fonts/pixel_font.ttf", 24);
    if (!font) {
        std::cerr << "Error cargando fuente: " << TTF_GetError() << std::endl;
    } else {
        std::cout << "Fuente cargada correctamente." << std::endl;
    }
}


void Hud::drawSpeed(SDL2pp::Renderer& renderer, float speed, const int windowWidth,
                    const int windowHeight) const {
    float speedKmh = speed;

    SDL2pp::Texture& tex = tm.getCars().getSpeedometer();
    SDL2pp::Rect dialRect = tm.getCars().getDialFrame();

    SDL2pp::Rect dstRectDial(windowWidth - 190, windowHeight - 190, 150, 150);
    renderer.Copy(tex, dialRect, dstRectDial);

   const float centerX = dstRectDial.x + dstRectDial.w / 2.0f;
    const float centerY = dstRectDial.y + dstRectDial.h / 2.0f;

    constexpr float minSpeed = 0.0f;
    constexpr float maxSpeed = 200.0f;
    constexpr float minAngle = -225.0f;
    constexpr float maxAngle = 45.0f;

    const float clampedSpeed = std::clamp(speedKmh, minSpeed, maxSpeed);
    const float angle = minAngle + (clampedSpeed - minSpeed) * (maxAngle - minAngle) / (maxSpeed - minSpeed);

    const float needleLength = dstRectDial.w * 0.4f;
    const float rad = angle * M_PI / 180.0f;
    const float endX = centerX + needleLength * cos(rad);
    const float endY = centerY + needleLength * sin(rad);

    renderer.SetDrawColor(255, 0, 0, 255);
    renderer.DrawLine(static_cast<int>(centerX), static_cast<int>(centerY),
                      static_cast<int>(endX), static_cast<int>(endY));

    renderer.SetDrawColor(0, 0, 0, 255);
    int radius = 4;
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if (dx * dx + dy * dy <= radius * radius) {
                renderer.DrawPoint(centerX + dx, centerY + dy);
            }
        }
    }

    const std::string speedText = std::to_string(static_cast<int>(clampedSpeed)) + " km/h";
    constexpr SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, speedText.c_str(), white);
    if (!textSurface) {
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer.Get(), textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        return;
    }

    const SDL_Rect textRect = {
        static_cast<int>(centerX - 40),
        dstRectDial.y + dstRectDial.h,
        80,
        30
    };

    SDL_FreeSurface(textSurface);

    SDL_RenderCopy(renderer.Get(), textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}
