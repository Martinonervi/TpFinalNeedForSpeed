#include "upgrade_screen.h"

UpgradeScreen::UpgradeScreen(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm,
    const int width, const int height)
    : renderer(renderer), drawer(drawer), tm(tm), width(width), height(height)
{

    buttons[0] = { {0, 0, 0, 0}, "", {0,0,0,0}, {255,255,255,100}, false };
    buttons[1] = { {0, 0, 0, 0}, "", {0,0,0,0}, {255,255,255,100}, false };
    buttons[2] = { {0, 0, 0, 0}, "", {0,0,0,0}, {255,255,255,100}, false };
}

void UpgradeScreen::renderPopUp(int windowWidth, int windowHeight) {
    int panelX = windowWidth/2 - width/2;
    int panelY = windowHeight/2 - height/2;
    SDL2pp::Rect panel = {panelX, panelY, width, height};

    // Overlay semi-transparente
    renderer.SetDrawColor(0, 0, 0, 220);
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.FillRect(panel);

    SDL2pp::Texture& slotTex = tm.getHud().getUpgradeFrame();

    auto& upgradesSheet = tm.getHud().getUpgrades();
    int iconSize = 30;

    int nitroX[3];
    int nitroY[3];
    int nitroW[3];
    int nitroH[3];
    nitroX[0] = 29;
    nitroX[1] = 63;
    nitroX[2] = 95;
    nitroY[0] = 1;
    nitroY[1] = 1;
    nitroY[2] = 1;
    nitroW[0] = 17;
    nitroW[1] = 16;
    nitroW[2] = 16;
    nitroH[0] = 17;
    nitroH[1] = 17;
    nitroH[2] = 17;


    for (int i = 0; i < 3; ++i) {
        int slotX = panelX + 50;
        int slotY = panelY + 50 + i*100;

        SDL2pp::Rect srcFrame(0, 0, slotTex.GetWidth(), slotTex.GetHeight());
        SDL2pp::Rect dstFrame(slotX, slotY, slotTex.GetWidth()*5, slotTex.GetHeight()*5);
        renderer.Copy(slotTex, srcFrame, dstFrame);

        Button& btn = buttons[i];
        btn.rect.x = slotX + 8*5;
        btn.rect.y = slotY + 7*5;
        btn.rect.w = 14*5;
        btn.rect.h = 14*5;
        btn.text = "";
        btn.color = {0,0,0,0};
        btn.hoverColor = {255,255,255,100};
        drawer.drawButton(btn);

        // Dibujar icono dentro del botón
        SDL2pp::Rect srcIcon(nitroX[i], nitroY[i], nitroW[i], nitroH[i]);
        SDL2pp::Rect dstIcon(btn.rect.x, btn.rect.y, 14*5, 14*5);
        renderer.Copy(upgradesSheet, srcIcon, dstIcon);
    }
}




void UpgradeScreen::handleMouseMotion(int mouseX, int mouseY) {
    // Actualizar hover de cada botón
    for (auto& btn : buttons) {
        btn.hover = (mouseX >= btn.rect.x && mouseX <= btn.rect.x + btn.rect.w &&
                     mouseY >= btn.rect.y && mouseY <= btn.rect.y + btn.rect.h);
    }
}

bool UpgradeScreen::handleMouseClick(int mouseX, int mouseY, std::string& clickedButton) {
    // Revisar si se hizo clic en algún botón
    for (auto& btn : buttons) {
        if (mouseX >= btn.rect.x && mouseX <= btn.rect.x + btn.rect.w &&
            mouseY >= btn.rect.y && mouseY <= btn.rect.y + btn.rect.h)
        {
            clickedButton = btn.text;
            return true;
        }
    }
    return false;
}
