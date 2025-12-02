#include "sdl_drawer.h"

#include "../../common_src/constants.h"

SdlDrawer::SdlDrawer(SDL2pp::Renderer& renderer) : renderer(renderer) {
    loadFont();
}


void SdlDrawer::drawCircle(const int cx, const int cy, const int radius) const{
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                renderer.DrawPoint(cx + dx, cy + dy);
            }
        }
    }
}

void SdlDrawer::drawText(const std::string& text, const int x, const int y, const SDL_Color color,
const float scaleX = 1.0f,
const float scaleY = 1.0f) const {
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "TTF_RenderText_Blended falló: " << TTF_GetError() << std::endl;
        return;
    }

    int w = surface->w;
    int h = surface->h;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.Get(), surface);

    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface falló: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect dstRect {
        x,
        y,
        static_cast<int>(w * scaleX),
        static_cast<int>(h * scaleY)
    };
    SDL_RenderCopy(renderer.Get(), texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);
}

void SdlDrawer::loadFont() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    }

    font = TTF_OpenFont(FONT_PATH, 24);
    if (!font) {
        std::cerr << "Error cargando fuente: " << TTF_GetError() << std::endl;
    } else {
        //std::cout << "Fuente cargada correctamente." << std::endl;
    }
}

void SdlDrawer::drawButton(const Button& button) const {

    const SDL2pp::Color color = button.getColor();

    renderer.SetDrawColor(color.r, color.g, color.b, color.a);
    renderer.FillRect(button.getRect());

    if (!button.getText().empty()) {
        if (!font) return;

        SDL_Surface* surface = TTF_RenderText_Blended(font, button.getText().c_str(), {255,255,255,255});
        if (!surface) return;

        int w = surface->w;
        int h = surface->h;
        SDL_FreeSurface(surface);

        const int textX = button.getRect().x + (button.getRect().w - w) / 2;
        const int textY = button.getRect().y + (button.getRect().h - h) / 2;

        drawText(button.getText(), textX, textY, {255, 255, 255, 255}, 1.0f, 1.0f);
    }
}

SDL2pp::Point SdlDrawer::getTextSize(const std::string& text) const {
    int w, h;
    TTF_SizeText(font, text.c_str(), &w, &h);
    return {w, h};
}
