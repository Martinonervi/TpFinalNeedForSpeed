#include "upgrade_screen.h"
#include "button.h"

UpgradeScreen::UpgradeScreen(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm,
    const int width, const int height, const int windowWidth, const int windowHeight)
    : renderer(renderer), drawer(drawer), tm(tm), width(width), height(height), windowWidth(windowWidth),
    windowHeight(windowHeight)
{}

void UpgradeScreen::renderPopUp() {
    const int panelX = windowWidth / 2 - width;
    const int panelY = 85;
    const SDL2pp::Rect panel = {panelX, panelY, width, height};

    renderer.SetDrawColor(0, 0, 0, 150);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.FillRect(panel);

    SDL2pp::Texture& slotTex = tm.getHud().getUpgradeFrame();
    auto& upgradesSheet = tm.getHud().getUpgrades();

    for (auto& upBtn : buttons) {
        const SDL2pp::Rect btnRect = upBtn.button.getRect();
        const int slotX = btnRect.x - 8 * 5;
        const int slotY = btnRect.y - 7*5;

        SDL2pp::Rect srcFrame(0, 0, slotTex.GetWidth(), slotTex.GetHeight());
        SDL2pp::Rect dstFrame(slotX, slotY,
                              slotTex.GetWidth()*5,
                              slotTex.GetHeight()*5);

        renderer.Copy(slotTex, srcFrame, dstFrame);

        drawer.drawButton(upBtn.button);    // Capaz le pueda meter draw?

        SDL2pp::Rect srcIcon = tm.getHud().getUpgradeIconRect(upBtn.type);
        SDL2pp::Rect dstIcon(btnRect.x, btnRect.y, 14*5, 14*5);
        renderer.Copy(upgradesSheet, srcIcon, dstIcon);
    }
}

void UpgradeScreen::handleMouseMotion(const int mouseX, const int mouseY) {
    for (auto& upBtn : buttons) {

        upBtn.button.handleHover(mouseX, mouseY);
    }
}

std::pair<bool, Upgrade> UpgradeScreen::handleMouseClick() {
    for (auto& upBtn : buttons) {

        if (upBtn.button.getHover()) {
            return {true, upBtn.type};
        }
    }
    return {false, NONE};
}

void UpgradeScreen::createButtons(const std::vector<UpgradeDef>& upgradesArray)
{
    const int panelX = windowWidth / 2 - width - 20;
    const int panelY = 85;
    buttons.clear();
    buttons.reserve(upgradesArray.size());

    const int spacing = 100;
    const int baseX = panelX + 10;
    const int baseY = panelY + 20;

    for (size_t i = 0; i < upgradesArray.size(); i++) {
        const UpgradeDef& up = upgradesArray[i];

        const int slotX = baseX;
        const int slotY = baseY + i * spacing;

        const SDL2pp::Rect btnRect(slotX + 8*5, slotY + 7*5, 14*5, 14*5);

        UpgradeButton upBtn{
            Button(btnRect, "",
                {0,0,0,0}, {255,255,255,100}),
            up.type
        };

        buttons.push_back(upBtn);
    }
}
