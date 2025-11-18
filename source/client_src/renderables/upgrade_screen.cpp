#include "upgrade_screen.h"

UpgradeScreen::UpgradeScreen(TextureManager& tm, SDL2pp::Renderer& renderer,
    const int width, const int height) : tm(tm), renderer(renderer), width(width), height(height)
{}

SDL2pp::Rect motorButton   = {50, 50, 150, 50};
SDL2pp::Rect ruedasButton  = {50, 120, 150, 50};
SDL2pp::Rect nitroButton   = {50, 190, 150, 50};


void UpgradeScreen::renderPopUp(int windowWidth, int windowHeight) {
    // Dibujar overlay semi-transparente
    renderer.SetDrawColor(0, 0, 0, 150); // negro semi-transparente
    SDL2pp::Rect panel = {windowWidth/2-width/2, windowHeight/2-height/2, width, height};
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.FillRect(panel);

    // Dibujar botones
    renderer.SetDrawColor(0, 255, 0, 255); // verde
    renderer.FillRect(motorButton);
    renderer.FillRect(ruedasButton);
    renderer.FillRect(nitroButton);

}


