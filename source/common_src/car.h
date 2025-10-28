#ifndef CAR_H
#define CAR_H

#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Renderer.hh>
#include "./car_sprites.h"

class Car {
public:
    Car(
        SDL2pp::Texture& tex,
        SDL2pp::Renderer& rend,
        int carIndex, int screenX,
        int screenY, int spd
        );

    void draw() const;
    void update(int dx, int dy);
    void move(int dx, int dy);

private:
    SDL2pp::Texture& texture;
    SDL2pp::Renderer& renderer;
    SDL_Rect dstRect{};
    int speed;

    int currentFrame = 0;
    int direction = 0;
    const CarSprite* sprite;
};


#endif
