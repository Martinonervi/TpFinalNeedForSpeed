#include "button.h"

Button::Button(SDL2pp::Rect rect, const std::string& text, SDL2pp::Color color, SDL2pp::Color hoverColor):
        rect(rect), text(text), color(color), hoverColor(hoverColor) {}

void Button::handleHover(int mouseX, int mouseY) {
    hover = (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                 mouseY >= rect.y && mouseY <= rect.y + rect.h);
}

SDL2pp::Rect Button::getRect() const {
    return rect;
}

bool Button::getHover() const {
    return hover;
}

SDL2pp::Color Button::getColor() const {
    return hover ? hoverColor : color;
}

std::string Button::getText() const {
    return text;
}