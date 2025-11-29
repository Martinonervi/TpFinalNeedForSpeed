#ifndef SERVER_DIS_SCREEN_H
#define SERVER_DIS_SCREEN_H
#include <SDL2pp/SDL2pp.hh>
#include "sdl_drawer.h"
#include "../sdl_constants.h"

class ServerDisScreen {
public:
    ServerDisScreen(int windowWidth, int windowHeight,
                    SDL2pp::Renderer& renderer,
                    SdlDrawer& drawer);

    void draw();

private:
    int windowWidth;
    int windowHeight;

    SDL2pp::Renderer& renderer;
    SdlDrawer& drawer;

};




#endif //SERVER_DIS_SCREEN_H
