#include "client_camera.h"

Camera::Camera(const int screenWidth, const int screenHeight, const int worldWidth,
               const int worldHeight)
        : screenWidth(screenWidth), screenHeight(screenHeight),
          worldWidth(worldWidth), worldHeight(worldHeight),
          x(0), y(0) {}

void Camera::follow(const int targetX, const int targetY) {
    x = targetX - screenWidth / 2;
    y = targetY - screenHeight / 2;

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + screenWidth > worldWidth) x = worldWidth - screenWidth;
    if (y + screenHeight > worldHeight) y = worldHeight - screenHeight;
}

int Camera::getX() const { return x; }
int Camera::getY() const { return y; }