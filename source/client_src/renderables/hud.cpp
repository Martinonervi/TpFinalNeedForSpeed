#include "hud.h"
#include <iostream>
#include <cmath>
#include <algorithm>

#define SIZE_OF_DIAL 150
#define DIAL_MARGIN 40

Hud::Hud(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, const MapType maptype)
    : map(maptype, renderer, tm), renderer(renderer), drawer(drawer), tm(tm)
{}


void Hud::drawOverlay(const int x, const int y,
                      std::unordered_map<ID, std::unique_ptr<Car>>& cars,
                      const ID playerId) const {

    map.draw(x, 10, cars, playerId);

    const auto it = cars.find(playerId);
    if (it == cars.end() || !it->second) return;

    const Car& playerCar = *it->second;
    const float health = playerCar.getHealth();
    float speed = playerCar.getSpeed();

    drawBars(renderer, x, health);
    drawDial(renderer, speed, x, y);

    // Por ahora uso nums falsos
    drawRaceNumber(3, 5);
    drawGameTime(523);
    activeUpgrade(x);
}

void Hud::drawDial(SDL2pp::Renderer& renderer, const float speed, const int windowWidth,
                    const int windowHeight) const {
    const float speedKmh = speed;

    SDL2pp::Texture& tex = tm.getCars().getSpeedometer();
    SDL2pp::Rect dialRect = tm.getCars().getDialFrame();

    SDL2pp::Rect dstRectDial(windowWidth - SIZE_OF_DIAL - DIAL_MARGIN,
        windowHeight - SIZE_OF_DIAL - DIAL_MARGIN, SIZE_OF_DIAL, SIZE_OF_DIAL);
    renderer.Copy(tex, dialRect, dstRectDial);

    const float centerX = dstRectDial.x + dstRectDial.w / 2.0f;
    const float centerY = dstRectDial.y + dstRectDial.h / 2.0f;

    const float clampedSpeed = drawNeedle(centerX, centerY, dstRectDial, speedKmh);

    drawSpeedText(clampedSpeed, dstRectDial, centerX);
}

float Hud::drawNeedle(const float x, const float y, const SDL2pp::Rect dstRectDial, const float speed) const {
    constexpr float minSpeed = 0.0f;
    constexpr float maxSpeed = 200.0f;
    constexpr float minAngle = -225.0f;
    constexpr float maxAngle = 45.0f;

    const float clampedSpeed = std::clamp(speed, minSpeed, maxSpeed);
    const float angle = minAngle + (clampedSpeed - minSpeed) * (maxAngle - minAngle) / (maxSpeed - minSpeed);

    const float needleLength = dstRectDial.w * 0.4f;
    const float rad = angle * M_PI / 180.0f;
    const float endX = x + needleLength * cos(rad);
    const float endY = y + needleLength * sin(rad);

    renderer.SetDrawColor(255, 0, 0, 255);
    renderer.DrawLine(static_cast<int>(x), static_cast<int>(y),
                      static_cast<int>(endX), static_cast<int>(endY));

    renderer.SetDrawColor(0, 0, 0, 255);
    constexpr int radius = 4;
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if (dx * dx + dy * dy <= radius * radius) {
                renderer.DrawPoint(x + dx, y + dy);
            }
        }
    }
    return clampedSpeed;
}

void Hud::drawSpeedText(const float clampedSpeed,
                        const SDL2pp::Rect dstRectDial,
                        const float x) const {

    std::string speedText = std::to_string(static_cast<int>(clampedSpeed)) + " km/h";
    SDL_Color white = {255, 255, 255, 255};

    // Coordenadas donde querés que aparezca
    int textX = static_cast<int>(x - 50);
    int textY = dstRectDial.y + dstRectDial.h;

    drawer.drawText(speedText, textX, textY, white);
}

void Hud::drawBars(SDL2pp::Renderer& renderer, const int windowWidth, const float health) const {
    constexpr int scale = 3;
    const int x = windowWidth / 3;
    const int y = 10*scale;

    SDL2pp::Texture& barsTexture = tm.getHud().getBarsTexture();

    constexpr float energyPercent = 50.0f/100; // 50% energía

    const SDL2pp::Rect healthSrc = tm.getHud().getHealthBar();
    SDL2pp::Rect healthDst(x, y, healthSrc.w*scale, healthSrc.h*scale);

    drawHealthFill(renderer, health, healthDst, scale);
    renderer.Copy(barsTexture,
                  healthSrc,
                  healthDst);

    const SDL2pp::Rect energySrc = tm.getHud().getEnergyBar();
    const int energyX = x + healthSrc.w*scale + 10;
    const int energyY = 5*scale;

    SDL2pp::Rect energyDst(energyX, energyY, energySrc.w*scale, energySrc.h*scale);

    drawEnergyFill(renderer, energyPercent, energyDst, scale);
    renderer.Copy(barsTexture, energySrc, energyDst);
}


void Hud::drawHealthFill(SDL2pp::Renderer& renderer, const float health, SDL2pp::Rect healthDst,
                         const int scale) const
{
    int startFillX = 14 * scale;
    int startFillY = 4 * scale;
    int filledW = (healthDst.w - startFillX - scale) * (health / 100.0f);

    const SDL2pp::Rect dst = {
        healthDst.x + startFillX,
        startFillY + healthDst.y,
        filledW,
        healthDst.h - startFillY
    };

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(255, 0, 77, 125);
    renderer.FillRect(dst);
}

void Hud::drawEnergyFill(SDL2pp::Renderer& renderer, float percent,
    const SDL2pp::Rect energyDst, int scale) const
{
    // Por fila: cuántos px hay realmente (forma escalera)
    static constexpr int ROW_W[5] = {53, 53, 51, 49, 47};
    constexpr int ROWS = 5;

    int startFillX = 10 * scale;
    int startFillY = 9 * scale;
    int filledW =  (energyDst.w - startFillX) * percent;

    int rowY = startFillY + energyDst.y;
    int rowH = (energyDst.h - startFillY)/ROWS;
    int rowW = filledW;

    for (int row = 0; row < ROWS; row++) {
        SDL2pp::Rect dst = {
            energyDst.x + startFillX,
            rowY,
            rowW,
            rowH,
        };
        rowY += rowH;
        rowW -= scale;
        renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
        renderer.SetDrawColor(255, 255, 0, 125);
        renderer.FillRect(dst);
    }

}

void Hud::drawGameTime(int totalSeconds) const {
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, minutes, seconds);

    SDL_Color yellow = {255, 255, 0, 255};
    drawer.drawText(buffer, 20, 65, yellow);
}

void Hud::drawRaceNumber(int current, int total) const {
    std::string txt = "Race " + std::to_string(current) + "/" + std::to_string(total);
    SDL_Color white = {255, 255, 255, 255};

    drawer.drawText(txt, 20, 35, white);
}

void Hud::activeUpgrade(const int windowWidth) const {
    SDL2pp::Texture& tex = tm.getHud().getUpgradeFrame();
    SDL2pp::Rect src(0, 0, tex.GetWidth()*4, tex.GetHeight()*4);
    const int x = windowWidth - 260 - src.w;
    SDL2pp::Rect dst(x, 10, src.w, src.h);

    renderer.Copy(tex, src, dst);
}




