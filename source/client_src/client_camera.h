#ifndef CLIENT_CAMERA_H
#define CLIENT_CAMERA_H
#include <SDL2pp/Texture.hh>

class Camera {
public:
    Camera(float screenWidth, float screenHeight, float worldWidth, float worldHeight);

    void follow(float targetX, float targetY);

    SDL2pp::Rect getView() const;
    int getX() const;
    int getY() const;

private:
    int x, y;
    int screenWidth, screenHeight;
    int worldWidth, worldHeight;
};




#endif //CLIENT_CAMERA_H
