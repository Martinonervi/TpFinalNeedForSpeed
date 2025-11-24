#include "upgrade_screen.h"

UpgradeScreen::UpgradeScreen(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm,
    const int width, const int height, const int windowWidth, const int windowHeight)
    : renderer(renderer), drawer(drawer), tm(tm), width(width), height(height), windowWidth(windowWidth),
    windowHeight(windowHeight)
{}


inline std::string upgradeTypeToString(const Upgrade type) {
    switch (type) {
        case EngineForce:
            return "EngineForce";
        default:
            return "Unknown";
    }
}


void UpgradeScreen::renderPopUp()
{
    int panelX = windowWidth/2 - width/2;
    int panelY = windowHeight/2 - height/2;
    SDL2pp::Rect panel = {panelX, panelY, width, height};

    renderer.SetDrawColor(0, 0, 0, 220);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.FillRect(panel);

    SDL2pp::Texture& slotTex = tm.getHud().getUpgradeFrame();
    auto& upgradesSheet = tm.getHud().getUpgrades();

    for (size_t i = 0; i < buttons.size(); i++) {

        // dibujar frame
        int slotX = buttons[i].button.rect.x - 8*5;
        int slotY = buttons[i].button.rect.y - 7*5;

        SDL2pp::Rect srcFrame(0, 0, slotTex.GetWidth(), slotTex.GetHeight());
        SDL2pp::Rect dstFrame(slotX, slotY,
                              slotTex.GetWidth()*5,
                              slotTex.GetHeight()*5);

        renderer.Copy(slotTex, srcFrame, dstFrame);

        // dibujar botón
        drawer.drawButton(buttons[i].button);

        // dibujar icono
        SDL2pp::Rect srcIcon = getUpgradeIconRect(buttons[i].type);
        SDL2pp::Rect dstIcon(buttons[i].button.rect.x, buttons[i].button.rect.y, 14*5, 14*5);
        renderer.Copy(upgradesSheet, srcIcon, dstIcon);
    }
}



SDL2pp::Rect UpgradeScreen::getUpgradeIconRect(const Upgrade type) {
    switch (type) {
        case EngineForce:
            return SDL2pp::Rect(29, 1, 17, 17);
        default:
            return SDL2pp::Rect(0, 0, 0, 0);
    }
}

void UpgradeScreen::handleMouseMotion(int mouseX, int mouseY) {
    // Actualizar hover de cada botón
    for (auto& upBtn : buttons) {
        Button& btn = upBtn.button;
        btn.hover = (mouseX >= btn.rect.x && mouseX <= btn.rect.x + btn.rect.w &&
                     mouseY >= btn.rect.y && mouseY <= btn.rect.y + btn.rect.h);
    }
}

std::pair<bool, Upgrade> UpgradeScreen::handleMouseClick(int mouseX, int mouseY, std::string& clickedButton) {
    for (auto& upBtn : buttons) {
        auto btn = upBtn.button;
        if (mouseX >= btn.rect.x && mouseX <= btn.rect.x + btn.rect.w &&
            mouseY >= btn.rect.y && mouseY <= btn.rect.y + btn.rect.h)
        {
            clickedButton = btn.text;
            return {true, upBtn.type};
        }
    }
    return {false, NONE};
}

void UpgradeScreen::createButtons(
        std::vector<UpgradeDef>& upgradesArray)
{
    int panelX = windowWidth/2 - width/2;
    int panelY = windowHeight/2 - height/2;
    buttons.clear();
    buttons.reserve(upgradesArray.size());

    const int spacing = 100;
    const int baseX = panelX + 50;
    const int baseY = panelY + 50;

    for (size_t i = 0; i < upgradesArray.size(); i++) {
        UpgradeDef& up = upgradesArray[i];

        // slot del frame
        int slotX = baseX;
        int slotY = baseY + i * spacing;

        // botón
        Button btn;
        btn.rect.x = slotX + 8*5;
        btn.rect.y = slotY + 7*5;
        btn.rect.w = 14*5;
        btn.rect.h = 14*5;
        btn.text = upgradeTypeToString(up.type);
        btn.color = {0,0,0,0};
        btn.hoverColor = {255,255,255,100};
        btn.hover = false;

        UpgradeButton upBtn;
        upBtn.button = btn;
        upBtn.type = up.type;

        buttons.push_back(upBtn);
    }
}
