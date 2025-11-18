#ifndef UPGRADE_SCREEN_H
#define UPGRADE_SCREEN_H

#include <SDL2pp/SDL2pp.hh>
#include "../textures/texture_manager.h"


class UpgradeScreen {
public:
    UpgradeScreen(TextureManager& tm, SDL2pp::Renderer& renderer, int width, int height);
    void renderPopUp(int windowWidth, int windowHeight);

    // Tipo de dato retornar eleccion

private:
    TextureManager& tm;
    SDL2pp::Renderer& renderer;

    int width;
    int height;

    // Upgrades

};



#endif //UPGRADE_SCREEN_H
