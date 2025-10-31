#ifndef CLIENT_CAMERA_H
#define CLIENT_CAMERA_H

class Camera {
public:
    Camera(int screenWidth, int screenHeight, int worldWidth, int worldHeight);

    void follow(int targetX, int targetY);

    int getX() const;
    int getY() const;

private:
    int x, y;
    int screenWidth, screenHeight;
    int worldWidth, worldHeight;
};




#endif //CLIENT_CAMERA_H
