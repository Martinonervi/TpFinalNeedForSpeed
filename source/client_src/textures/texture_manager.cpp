#include "texture_manager.h"
#include <cmath>

TextureManager::TextureManager(SDL2pp::Texture& carsTexture): carsTexture(carsTexture) {
    carInfos[CAR_GREEN] = {0, SMALL_CAR, SMALL_CAR};
    carInfos[CAR_RED] = {64, MEDIUM_CAR, MEDIUM_CAR};
    carInfos[CAR_PORSCHE] = {144, MEDIUM_CAR, MEDIUM_CAR};
    carInfos[CAR_LIGHT_BLUE] = {224, MEDIUM_CAR, MEDIUM_CAR};
    carInfos[CAR_JEEP] = {304, MEDIUM_CAR, MEDIUM_CAR};
    carInfos[CAR_PICKUP] = {384, MEDIUM_CAR, MEDIUM_CAR};
    carInfos[CAR_LIMO] = {464, LARGE_CAR, LARGE_CAR};
}

SDL2pp::Rect TextureManager::getCarFrame(const CarType type, const float angle) {
    const auto& [yOffset, width, height] = carInfos[type];

    const float adjusted = angle + M_PI / 2.0f;

    float normalized = fmod(adjusted, 2.0f * M_PI);
    if (normalized < 0)
        normalized += 2.0f * M_PI;

    const int frameIndex =
        static_cast<int>(normalized / (2.0f * M_PI) * FRAMES_PER_CAR) % FRAMES_PER_CAR;

    const int col = frameIndex % FRAMES_PER_DIRECTION;
    const int row = frameIndex >= FRAMES_PER_DIRECTION ? 1 : 0;
    const int x = col * width;
    const int y = yOffset + row * height;

    return {x, y, width, height};
}


SDL2pp::Texture& TextureManager::getCarsTexture() const {
    return carsTexture;
}