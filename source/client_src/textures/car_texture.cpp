#include "car_texture.h"
#include <cmath>

CarTexture::CarTexture(SDL2pp::Texture& carsTexture, SDL2pp::Texture& speedometerTexture)
    : carsTexture(carsTexture), speedometerTexture(speedometerTexture) {
    carInfo[CAR_GREEN] = {0, SMALL_CAR, SMALL_CAR};
    carInfo[CAR_RED] = {64, MEDIUM_CAR, MEDIUM_CAR};
    carInfo[CAR_PORSCHE] = {144, MEDIUM_CAR, MEDIUM_CAR};
    carInfo[CAR_LIGHT_BLUE] = {224, MEDIUM_CAR, MEDIUM_CAR};
    carInfo[CAR_JEEP] = {304, MEDIUM_CAR, MEDIUM_CAR};
    carInfo[CAR_PICKUP] = {384, MEDIUM_CAR, MEDIUM_CAR};
    carInfo[CAR_LIMO] = {464, LARGE_CAR, LARGE_CAR};
}

SDL2pp::Rect CarTexture::getFrame(const CarType type, const float angle) const {
    const auto& info = carInfo.at(type);
    const auto& [yOffset, width, height] = info;

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

SDL2pp::Texture& CarTexture::getTexture() const {
    return carsTexture;
}

SDL2pp::Texture& CarTexture::getSpeedometer() const {
    return speedometerTexture;
}

SDL2pp::Rect CarTexture::getNeedleFrame(float speed) const {
    const int FRAMES = 8;           // cantidad de agujas
    const float MAX_SPEED = 240.0f; // velocidad máxima
    const int FRAME_WIDTH = 16;     // ancho aprox de cada aguja
    const int FRAME_HEIGHT = 16;    // alto aprox de la tira superior

    // Determinamos qué frame usar
    if (speed < 0) speed = 0;
    if (speed > MAX_SPEED) speed = MAX_SPEED;

    int frameIndex = static_cast<int>((speed / MAX_SPEED) * (FRAMES - 1));

    // Calcular la posición en la tira
    int x = 13 + frameIndex * FRAME_WIDTH;
    int y = 7; // las agujas están arriba

    return SDL2pp::Rect(x, y, FRAME_WIDTH, FRAME_HEIGHT);
}

SDL2pp::Rect CarTexture::getDialFrame() const {
    const int WIDTH = 87;
    const int HEIGHT = 87;
    const int X = 10;
    const int Y = 29;
    return SDL2pp::Rect(X, Y, WIDTH, HEIGHT);
}
