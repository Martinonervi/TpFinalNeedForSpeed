#ifndef CLIENT_CAMERA_H
#define CLIENT_CAMERA_H
#include <SDL2pp/Texture.hh>

class Camera {
public:
    Camera(int screenWidth, int screenHeight, int worldWidth, int worldHeight);

    void follow(int targetX, int targetY);

    SDL2pp::Rect getView() const;

private:
    int x, y;
    int screenWidth, screenHeight;
    int worldWidth, worldHeight;
};




#endif //CLIENT_CAMERA_H
