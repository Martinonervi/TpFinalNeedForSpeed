#include "sdl_drawer.h"

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

void SdlDrawer::drawText(const std::string& text, const int x, const int y, const SDL_Color color) const {
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

    SDL_Rect textRect { x, y, w, h };
    SDL_RenderCopy(renderer.Get(), texture, nullptr, &textRect);
    SDL_DestroyTexture(texture);
}

void SdlDrawer::loadFont() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    }

    font = TTF_OpenFont("../assets/fonts/pixel_font.ttf", 24);
    if (!font) {
        std::cerr << "Error cargando fuente: " << TTF_GetError() << std::endl;
    } else {
        std::cout << "Fuente cargada correctamente." << std::endl;
    }
}

void SdlDrawer::drawButton(const Button button) const {

    SDL_Color color = button.hover ? button.hoverColor : button.color;

    renderer.SetDrawColor(color.r, color.g, color.b, color.a);
    renderer.FillRect(button.rect);

    if (!button.text.empty()) {
        if (!font) return;

        SDL_Surface* surface = TTF_RenderText_Blended(font, button.text.c_str(), {255,255,255,255});
        if (!surface) return;

        int w = surface->w;
        int h = surface->h;
        SDL_FreeSurface(surface);

        int textX = button.rect.x + (button.rect.w - w) / 2;
        int textY = button.rect.y + (button.rect.h - h) / 2;

        drawText(button.text, textX, textY, {255, 255, 255, 255});
    }
}
