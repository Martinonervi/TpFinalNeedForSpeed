#include "client_camera.h"

Camera::Camera(const float screenWidth, const float screenHeight, const float worldWidth,
               const float worldHeight)
        : x(0), y(0),
          screenWidth(screenWidth), screenHeight(screenHeight),
          worldWidth(worldWidth), worldHeight(worldHeight) {}

void Camera::follow(const float targetX, const float targetY) {
    x = targetX - screenWidth / 2;
    y = targetY - screenHeight / 2;

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + screenWidth > worldWidth) x = worldWidth - screenWidth;
    if (y + screenHeight > worldHeight) y = worldHeight - screenHeight;
}

SDL2pp::Rect Camera::getView() const {
    return {x, y, screenWidth, screenHeight};
}

int Camera::getX() const { return x; }
int Camera::getY() const { return y; }
