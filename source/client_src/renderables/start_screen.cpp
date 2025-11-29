#include "start_screen.h"


StartScreen::StartScreen(
    SDL2pp::Renderer& renderer, SdlDrawer& drawer,
    TextureManager& tm, const MapType maptype,
    std::vector<RecommendedPoint>& pathArray,
    UpgradeScreen& ups,
    Button& startBtn
    )
    : map(maptype, renderer, tm, pathArray), renderer(renderer), drawer(drawer), tm(tm),
        ups(ups), startBtn(startBtn)
{}

void StartScreen::draw(const int windowWidth, const int windowHeight) {
    SDL2pp::Texture& tex = tm.getCities().getStartBackTexture();

    SDL2pp::Rect dst(
        0,
        0,
        windowWidth,
        windowHeight
    );

    renderer.Copy(tex, SDL2pp::NullOpt, dst);

    SDL2pp::Rect backStartBtn(windowWidth/2 - 155,
        windowHeight/10*8 - 5,
        310, 90);

    renderer.SetDrawColor(0, 102, 0, 255);
    renderer.FillRect(backStartBtn);

    drawer.drawButton(startBtn);

    std::unordered_map<ID, std::unique_ptr<Car>> emptyCars;

    SDL2pp::Rect backMapBtn(windowWidth/2 + 5, 85, 480, 480);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(10, 10, 10, 150);
    renderer.FillRect(backMapBtn);

    map.draw(windowWidth/2 + 20, 100, emptyCars, -1, 150, 450);

    ups.renderPopUp();
}