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

void StartScreen::draw(const int windowWidth, const int windowHeight) const {
    SDL2pp::Texture& tex = tm.getCities().getStartBackTexture();

    SDL2pp::Rect dst(
        DEFAULT,
        DEFAULT,
        windowWidth,
        windowHeight
    );

    renderer.Copy(tex, SDL2pp::NullOpt, dst);

    if (start) {
        drawStartButton(windowWidth, windowHeight);
    } else {
        drawCountdown(windowWidth, windowHeight);
    }

    drawer.drawText(
        NEXT_ROUTE_TXT,
        windowWidth/2 + 30,
        40,
        WHITE,
        1.0f,
        1.0f
    );

    drawMap(windowWidth);

    drawer.drawText(
        UP_SHOP_TXT,
        windowWidth / 2 - 420,
        40,
        WHITE,
        1.0f,
        1.0f
    );

    ups.renderPopUp();
}

void StartScreen::drawStartButton(const int windowWidth, const int windowHeight) const {
    const SDL2pp::Rect backStartBtn(windowWidth/2 - 155,
        windowHeight/10*8 - 5,
        310, 90);

    renderer.SetDrawColor(0, 102, 0, 255);
    renderer.FillRect(backStartBtn);

    drawer.drawButton(startBtn);
}

void StartScreen::drawMap(const int windowWidth) const {
    std::unordered_map<ID, std::unique_ptr<Car>> emptyCars;

    const SDL2pp::Rect backMapBtn(windowWidth/2 + 5, 85, 480, 480);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(10, 10, 10, 150);
    renderer.FillRect(backMapBtn);

    map.draw(windowWidth/2 + 20, 100, emptyCars, -1, 150, 450);
}

void StartScreen::setTimeLeft(const uint8_t time) {
    timeLeft = time;
}

bool StartScreen::isStart() const {
    return start;
}

void StartScreen::changeIsStart() {
    start = !start;
}

void StartScreen::drawCountdown(const int windowWidth, const int windowHeight) const {
    int t = static_cast<int>(timeLeft);
    int minutes = t / 60;
    int seconds = t % 60;

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);

    drawer.drawText(
        buffer,
        windowWidth/2 - 155 + 90,
        windowHeight/10*8 - 5 + 30,
        {255,255,255,255},
        1.0f,
        1.0f
    );
}
