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

    for (int i = 0; i < 3; ++i) {
        int slotX = panelX + 50;
        int slotY = panelY + 50 + i*70;

        SDL2pp::Rect src(0, 0, slotTex.GetWidth(), slotTex.GetHeight());
        SDL2pp::Rect dst(slotX, slotY, slotTex.GetWidth()*4, slotTex.GetHeight()*4); // escalada *4
        renderer.Copy(slotTex, src, dst);

        Button& btn = buttons[i];
        btn.rect.x = slotX + 8*4;
        btn.rect.y = slotY + 7*4;
        btn.rect.w = 14*4;
        btn.rect.h = 14*4;

        drawer.drawButton(btn);

        // drawIcon(btn.rect.x, btn.rect.y, iconTexture);
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

