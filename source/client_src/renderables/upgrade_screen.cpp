#include "upgrade_screen.h"

#include "../sdl_constants.h"

#include "button.h"

UpgradeScreen::UpgradeScreen(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm,
    const int width, const int height, const int windowWidth, const int windowHeight)
    : renderer(renderer), drawer(drawer), tm(tm), width(width), height(height), windowWidth(windowWidth),
    windowHeight(windowHeight)
{}

void UpgradeScreen::renderPopUp() const {
    const int panelX = windowWidth / 2 - width;
    const int panelY = 85;
    const SDL2pp::Rect panel = {panelX, panelY, width, height};

    renderer.SetDrawColor(BLACK.r, BLACK.g, BLACK.b, 150);
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

        const int dstX = btnRect.x + (btnRect.w - upBtn.iconRect.w*4)/2;
        const int dstY = btnRect.y + (btnRect.h - upBtn.iconRect.h*4)/2;
        SDL2pp::Rect dstIcon(
            dstX, dstY,
            upBtn.iconRect.w*4, upBtn.iconRect.h*4)
        ;
        renderer.Copy(upgradesSheet, upBtn.iconRect, dstIcon);

        drawer.drawButton(upBtn.button);    // Capaz le pueda meter draw?

        auto it = upgradeDescriptions.find(upBtn.type);
        std::string desc = (it != upgradeDescriptions.end()) ? it->second : "Sin descripción";
        writeDescription(upBtn.penalty, desc, slotX + slotTex.GetWidth()*5 , slotY);
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
    std::unordered_map<Upgrade, bool> existing;
    for (auto& upBtn : buttons) {
        existing[upBtn.type] = true;
    }

    const int panelX = windowWidth / 2 - width - 20;
    const int panelY = 85;
    const int spacing = 100;
    const int baseX = panelX + 10;
    const int baseY = panelY + 20;

    for (auto up : upgradesArray) {
        if (existing.find(up.type) != existing.end()) continue;

        const int slotX = baseX;
        const int slotY = baseY + buttons.size() * spacing;

        const SDL2pp::Rect btnRect(slotX + 8 * 5, slotY + 7 * 5, 14 * 5, 14 * 5);
        const SDL2pp::Rect iconRect = tm.getHud().getUpgradeIconRect(up.type);
        UpgradeButton upBtn{
            Button(btnRect, NO_TEXT, {0,0,0,0},
                {WHITE.r,WHITE.g,WHITE.b,100}),
            up.type,
            up.penaltySec,
            iconRect
        };

        buttons.push_back(upBtn);
    }
}

void UpgradeScreen::writeDescription(const float penalty, const std::string& desc, const int x, const int y) const {

    drawer.drawText(desc, x - 28, y + 40, WHITE, 0.45f, 0.65f);

    char buf[64];
    std::snprintf(buf, sizeof(buf), TIME_PENALTY, penalty);

    drawer.drawText(buf, x - 25, y + 75, RED, 0.45f, 0.65f);
}

void UpgradeScreen::changeState(const Upgrade upgrade) {
    for (auto& upBtn : buttons) {
        if (upBtn.type == upgrade) {
            auto rect = upBtn.iconRect;
            upBtn.iconRect = {rect.x + rect.w, rect.y, rect.w, rect.h};
        }
    }
}

