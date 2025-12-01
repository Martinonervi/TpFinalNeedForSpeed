#include "server_dis_screen.h"

ServerDisScreen::ServerDisScreen(int windowWidth, int windowHeight,
                                 SDL2pp::Renderer& renderer,
                                 SdlDrawer& drawer)
    : windowWidth(windowWidth),
      windowHeight(windowHeight),
      renderer(renderer),
      drawer(drawer)
 {}

void ServerDisScreen::draw() {
    const int w = windowWidth / 2;
    const int h = windowHeight / 6;
    const int x = (windowWidth - w) / 2;
    const int y = (windowHeight - h) / 2;

    SDL2pp::Rect rect(x, y, w, h);

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(BLACK.r, BLACK.g, BLACK.b, 180);
    renderer.FillRect(rect);

    renderer.SetDrawColor(WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    renderer.DrawRect(rect);

    drawer.drawText(
        SRV_DISC_TXT,
        x + (w * 0.1f) + 35,
        y + (h * 0.35f),
        {WHITE},
        0.8f,
        1.0f
    );
}
