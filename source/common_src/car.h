#ifndef CAR_H
#define CAR_H

#include <SDL2pp/SDL2pp.hh>
#include <SDL2pp/Renderer.hh>

class Car {
private:
    SDL2pp::Texture& texture;   // spritesheet
    SDL2pp::Renderer& renderer; // renderer
    SDL_Rect srcRect{};              // coordenadas dentro del spritesheet
    SDL_Rect dstRect{};              // posición y tamaño en la ventana
    int speed;                     // velocidad del auto

public:
    Car( SDL2pp::Texture& tex, SDL2pp::Renderer& rend,
         int x, int y, int w, int h,
         int screenX, int screenY, int spd = 5);

    void draw() const;

    void move(int dx, int dy);

    int getX() const;
    int getY() const;

    void setPosition(int x, int y);

    void setSpeed(int spd);
    int getSpeed() const;
};


#endif
