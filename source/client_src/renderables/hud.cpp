#include "hud.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "../sdl_constants.h"

Hud::Hud(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, const MapType maptype,
    std::vector<RecommendedPoint>& pathArray)
    : map(maptype, renderer, tm, pathArray), drawer(drawer), renderer(renderer), tm(tm)
{}


void Hud::drawOverlay(const int x, const int y,
                      std::unordered_map<ID, std::unique_ptr<Car>>& cars,
                      const ID playerId, const int raceTime,
                      const uint8_t totalRaces, const uint8_t raceNumber,
                      const uint8_t totalCheckpoints, const ID checkpointNumber, const int countdown,
                      const uint8_t ranking ) const {

    if (countdown != NOT_ACCESSIBLE) drawCountdown(countdown, x, y);
    const auto it = cars.find(playerId);
    if (it == cars.end() || !it->second) return;

    const Car& playerCar = *it->second;

    const float px = playerCar.getX();
    const float py = playerCar.getY();

    int mapX = x - MINIMAP_SIZE - HUD_MARGIN;
    int mapY = HUD_MARGIN;

    if (px > x/2 && py < y/2) {
        mapX = HUD_MARGIN;
        mapY = y - MINIMAP_SIZE - HUD_MARGIN;
    }

    map.draw(mapX, mapY, cars, playerId, 150, MINIMAP_SIZE);

    const auto upgrades = it->second->getUpgrades();
    const float healthPerc = playerCar.getHealthPercentage();
    const float speed = playerCar.getSpeed();

    drawBars(renderer, x, healthPerc);
    drawDial(renderer, speed, x, y);

    drawRaceNumber(raceNumber, totalRaces);
    drawRanking(ranking);
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
    const float clampedSpeed = std::clamp(speed, MIN_SPEED, MAX_SPEED);
    const float angle =
        MIN_NEEDLE_ANGLE + (clampedSpeed - MIN_SPEED) * (MAX_NEEDLE_ANGLE - MIN_NEEDLE_ANGLE) / (MAX_SPEED - MIN_SPEED);

    const float needleLength = dstRectDial.w * 0.4f;
    const float rad = angle * M_PI / 180.0f;
    const float endX = x + needleLength * cos(rad);
    const float endY = y + needleLength * sin(rad);

    renderer.SetDrawColor(RED);
    renderer.DrawLine(static_cast<int>(x), static_cast<int>(y),
                      static_cast<int>(endX), static_cast<int>(endY));

    renderer.SetDrawColor(BLACK.r, BLACK.g, BLACK.b, BLACK.a);
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

    const int textX = static_cast<int>(x - 50);
    const int textY = dstRectDial.y + dstRectDial.h;

    drawer.drawText(speedText, textX - 26, textY, WHITE, 1.0f, 1.0f);
}

void Hud::drawBars(SDL2pp::Renderer& renderer, const int windowWidth, const float healthPerc) const {
    constexpr int scale = 3;
    const int x = windowWidth / 3;
    constexpr int y = HUD_MARGIN*scale;

    SDL2pp::Texture& barsTexture = tm.getHud().getBarsTexture();

    const SDL2pp::Rect healthSrc = tm.getHud().getHealthBar();
    SDL2pp::Rect healthDst(x, y, healthSrc.w*scale, healthSrc.h*scale);

    drawHealthFill(renderer, healthPerc, healthDst, scale);
    renderer.Copy(barsTexture,
                  healthSrc,
                  healthDst);
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
    renderer.SetDrawColor(RED_HEALTH_FILL);
    renderer.FillRect(dst);
}

void Hud::drawGameTime(const int totalSeconds) const {
    const int hours = totalSeconds / HOURS_TO_SECS;
    const int minutes = (totalSeconds % HOURS_TO_SECS) / MINS_TO_SECS;
    const int seconds = totalSeconds % MINS_TO_SECS;

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, minutes, seconds);

    drawer.drawText(buffer, 20, 105, YELLOW, 0.7f, 0.75f);
}


void Hud::drawRaceNumber(const int current, const int total) const {
    const std::string txt = RACE_TXT + std::to_string(current) + BACK_SLASH + std::to_string(total);

    drawer.drawText(txt, 20, 45, WHITE, 0.8f, 0.8f);
}

void Hud::drawCheckpointNumber(const int current, const int total) const {
    const std::string txt = CHECK_TXT + std::to_string(current) + BACK_SLASH + std::to_string(total);

    drawer.drawText(txt, 20, 75, WHITE, 0.8f, 0.8f);
}

void Hud::activeUpgrades(const int windowWidth, const std::vector<Upgrade>& upgrades) const {
    for (int i = 0; i < MAX_UPGRADE_FRAMES; i++) {
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

    constexpr int totalWidth = MAX_UPGRADE_FRAMES * realW + (MAX_UPGRADE_FRAMES - 1) * spacing;
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

void Hud::drawCountdown(const int countdown, const int width, const int height) const {
    const std::string txt = std::to_string(countdown);

    float scaleX = 2.f;
    float scaleY = 2.f;

    auto [textW, textH] = drawer.getTextSize(txt);

    int drawW = textW * scaleX;
    int drawH = textH * scaleY;

    int x = (width - drawW) / 2;
    int y = (height - drawH) / 2;

    drawer.drawText(txt, x, y, WHITE, 2.f, 2.f);
}


void Hud::drawRanking(const uint8_t ranking) const {
    const std::string txt = RANKING_TXT + std::to_string(ranking);

    drawer.drawText(txt, 20, 15, YELLOW, 0.8f, 0.8f);
}