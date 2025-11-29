#include "hud.h"
#include <iostream>
#include <cmath>
#include <algorithm>

#define SIZE_OF_DIAL 150
#define DIAL_MARGIN 40
#define MAX_FRAMES 3

Hud::Hud(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, const MapType maptype,
    std::vector<RecommendedPoint>& pathArray)
    : map(maptype, renderer, tm, pathArray), drawer(drawer), renderer(renderer), tm(tm)
{}


void Hud::drawOverlay(const int x, const int y,
                      std::unordered_map<ID, std::unique_ptr<Car>>& cars,
                      const ID playerId, const float raceTime,
                      const uint8_t totalRaces, const uint8_t raceNumber,
                      const uint8_t totalCheckpoints, const ID checkpointNumber) const {

    const auto it = cars.find(playerId);
    if (it == cars.end() || !it->second) return;

    const Car& playerCar = *it->second;

    float px = playerCar.getX();
    float py = playerCar.getY();

    int mapX = x - 260;
    int mapY = 10;

    if (px > x/2 && py < y/2) {
        mapX = 10;
        mapY = y - 250 - 10;
    }

    map.draw(mapX, mapY, cars, playerId, 150, 250);

    const auto upgrades = it->second->getUpgrades();
    const float healthPerc = playerCar.getHealthPercentage();
    float speed = playerCar.getSpeed();

    drawBars(renderer, x, healthPerc);
    drawDial(renderer, speed, x, y);

    drawRaceNumber(raceNumber, totalRaces);
    drawCheckpointNumber(checkpointNumber, totalCheckpoints);
    drawGameTime(raceTime);
    activeUpgrades(x, upgrades);
}

void Hud::drawDial(SDL2pp::Renderer& renderer, const float speed, const int windowWidth,
                    const int windowHeight) const {
    const float speedKmh = speed*2;

    SDL2pp::Texture& tex = tm.getCars().getSpeedometer();
    SDL2pp::Rect dialRect = tm.getCars().getDialFrame();

    SDL2pp::Rect dstRectDial(windowWidth - SIZE_OF_DIAL - DIAL_MARGIN,
        windowHeight - SIZE_OF_DIAL - DIAL_MARGIN, SIZE_OF_DIAL, SIZE_OF_DIAL);
    renderer.Copy(tex, dialRect, dstRectDial);

    const float centerX = dstRectDial.x + dstRectDial.w / 2.0f;
    const float centerY = dstRectDial.y + dstRectDial.h / 2.0f;

    const float clampedSpeed = drawNeedle(centerX, centerY, dstRectDial, speedKmh);

    drawSpeedText(speedKmh, dstRectDial, centerX);
}

float Hud::drawNeedle(const float x, const float y, const SDL2pp::Rect dstRectDial, const float speed) const {
    constexpr float minSpeed = 0.0f;
    constexpr float maxSpeed = 80.0f;
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

    const std::string speedText = std::to_string(static_cast<int>(clampedSpeed)) + " km/h";
    constexpr SDL2pp::Color white = {255, 255, 255, 255};

    const int textX = static_cast<int>(x - 50);
    const int textY = dstRectDial.y + dstRectDial.h;

    drawer.drawText(speedText, textX - 26, textY, white, 1.0f, 1.0f);
}

void Hud::drawBars(SDL2pp::Renderer& renderer, const int windowWidth, const float healthPerc) const {
    constexpr int scale = 3;
    const int x = windowWidth / 3;
    const int y = 10*scale;

    SDL2pp::Texture& barsTexture = tm.getHud().getBarsTexture();

    constexpr float energyPercent = 50.0f/100; // 50% energía

    const SDL2pp::Rect healthSrc = tm.getHud().getHealthBar();
    SDL2pp::Rect healthDst(x, y, healthSrc.w*scale, healthSrc.h*scale);

    drawHealthFill(renderer, healthPerc, healthDst, scale);
    renderer.Copy(barsTexture,
                  healthSrc,
                  healthDst);

    // Esto es para dibujar una barra similar a la de vida, al final no lo usamos pero lo dejo por las dudas

    /*
    const SDL2pp::Rect energySrc = tm.getHud().getEnergyBar();
    const int energyX = x + healthSrc.w*scale + 10;
    const int energyY = 5*scale;

    SDL2pp::Rect energyDst(energyX, energyY, energySrc.w*scale, energySrc.h*scale);

    drawEnergyFill(renderer, energyPercent, energyDst, scale);
    renderer.Copy(barsTexture, energySrc, energyDst);
    */
}


void Hud::drawHealthFill(SDL2pp::Renderer& renderer, const float healthPerc, SDL2pp::Rect healthDst,
                         const int scale) const
{
    int startFillX = 14 * scale;
    int startFillY = 4 * scale;
    int filledW = (healthDst.w - startFillX - scale) * healthPerc;

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

void Hud::drawGameTime(const int totalSeconds) const {
    const int hours = totalSeconds / 3600;
    const int minutes = (totalSeconds % 3600) / 60;
    const int seconds = totalSeconds % 60;

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, minutes, seconds);

    constexpr SDL2pp::Color yellow = {255, 255, 0, 255};
    drawer.drawText(buffer, 20, 75, yellow, 0.7f, 0.75f);
}

// MODULARIZAR

void Hud::drawRaceNumber(int current, int total) const {
    const std::string txt = "Race " + std::to_string(current) + "/" + std::to_string(total);
    constexpr SDL2pp::Color white = {255, 255, 255, 255};

    drawer.drawText(txt, 20, 15, white, 0.8f, 0.8f);
}

void Hud::drawCheckpointNumber(const int current, const int total) const {
    const std::string txt = "Checkpoint " + std::to_string(current) + "/" + std::to_string(total);
    constexpr SDL2pp::Color white = {255, 255, 255, 255};

    drawer.drawText(txt, 20, 45, white, 0.8f, 0.8f);
}

// -- - - -- - - -

void Hud::activeUpgrades(const int windowWidth, const std::vector<Upgrade>& upgrades) const {
    for (int i = 0; i < MAX_FRAMES; i++) {
        const Upgrade up = (i < upgrades.size() ? upgrades[i] : NONE);
        drawUpgrade(windowWidth, up, i);
    }
}



void Hud::drawUpgrade(const int windowWidth, const Upgrade upgrade, const int i) const {
    SDL2pp::Texture& slotTex = tm.getHud().getUpgradeFrame();
    auto& upgradesSheet = tm.getHud().getUpgrades();

    constexpr int frameScale = 4;
    constexpr int iconScale  = 3;

    const int frameW = slotTex.GetWidth()  * frameScale;
    const int frameH = slotTex.GetHeight() * frameScale;

    constexpr int realW = 18 * frameScale;
    constexpr int realH = 18 * frameScale;

    constexpr int spacing = 15;

    constexpr int totalWidth = MAX_FRAMES * realW + (MAX_FRAMES - 1) * spacing;
    const int startX = (windowWidth - totalWidth) / 2 + 190;
    const int y = 0;
    const int x = startX + i * (realW + spacing);

    SDL2pp::Rect srcFrame(0, 0, slotTex.GetWidth(), slotTex.GetHeight());
    SDL2pp::Rect dstFrame(x, y, frameW, frameH);
    renderer.Copy(slotTex, srcFrame, dstFrame);

    SDL2pp::Rect srcIcon = tm.getHud().getUpgradeIconRect(upgrade);

    const int dstX = x + 6*frameScale + (realW - srcIcon.w*iconScale)/2;
    const int dstY = y + 5*frameScale + (realH - srcIcon.h*iconScale)/2;

    SDL2pp::Rect dstIcon(
        dstX,
        dstY,
        srcIcon.w * iconScale,
        srcIcon.h * iconScale
    );

    renderer.Copy(upgradesSheet, srcIcon, dstIcon);
}



