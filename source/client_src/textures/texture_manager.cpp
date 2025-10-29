#include "texture_manager.h"
#include <cmath>

TextureManager::TextureManager(SDL2pp::Renderer& renderer)
    : renderer(renderer)
{
    carInfos[CAR_GREEN]      = { 0, SMALL_CAR, SMALL_CAR };
    carInfos[CAR_RED]        = { 64, MEDIUM_CAR, MEDIUM_CAR };
    carInfos[CAR_PORSCHE]    = { 144, MEDIUM_CAR, MEDIUM_CAR };
    carInfos[CAR_LIGHT_BLUE] = { 224, MEDIUM_CAR, MEDIUM_CAR };
    carInfos[CAR_JEEP]       = { 304, MEDIUM_CAR, MEDIUM_CAR };
    carInfos[CAR_PICKUP]     = { 384, MEDIUM_CAR, MEDIUM_CAR };
    carInfos[CAR_LIMO]       = { 464, LARGE_CAR, LARGE_CAR };
}

void TextureManager::loadCarsTexture(const std::string& path) {
    carsTexture = SDL2pp::Texture(renderer, path);
}

SDL2pp::Rect TextureManager::getCarFrame(const CarType type, const float angle) {
    if (type < 0 || type >= CAR_COUNT)
        throw std::runtime_error("Invalid CarType");

    const CarInfo& info = carInfos[type];

    float normalized = fmod(angle, 2.0f * M_PI);
    if (normalized < 0) normalized += 2.0f * M_PI;

    // Calculamos frame
    int frameIndex = static_cast<int>(normalized / (2.0f * M_PI) * FRAMES_PER_CAR) % FRAMES_PER_CAR;

    int x = frameIndex * info.width;
    int y = f;

    return SDL2pp::Rect(x, y, info.width, info.height);
}
