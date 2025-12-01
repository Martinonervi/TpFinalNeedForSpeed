#ifndef BUTTON_H
#define BUTTON_H

#include <string>

#include <SDL2pp/SDL2pp.hh>

class Button {
public:
    Button(SDL2pp::Rect rect, const std::string& text, SDL2pp::Color color, SDL2pp::Color hoverColor);
    void handleHover(int mouseX, int mouseY);
    SDL2pp::Rect getRect() const;
    bool getHover() const;
    SDL_Color getColor() const;
    std::string getText() const;

private:
    SDL2pp::Rect rect;
    std::string text;
    SDL2pp::Color color;
    SDL2pp::Color hoverColor;
    bool hover = false;

};

#endif
