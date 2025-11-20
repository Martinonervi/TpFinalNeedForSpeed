#ifndef SDL_DRAWER_H
#define SDL_DRAWER_H

#include <SDL2pp/SDL2pp.hh>

struct Button {
    SDL_Rect rect;
    std::string text;
    SDL_Color color;
    SDL_Color hoverColor;
    bool hover = false;
};

class SdlDrawer {
public:
    explicit SdlDrawer(SDL2pp::Renderer& renderer);
    void drawText(const std::string& text, int x, int y, SDL_Color color) const;
    void drawCircle(int cx, int cy, int radius) const;
    void drawButton(Button button) const;

private:
    TTF_Font* font;
    SDL2pp::Renderer& renderer;

    void loadFont();

};



#endif //SDL_DRAWER_H
