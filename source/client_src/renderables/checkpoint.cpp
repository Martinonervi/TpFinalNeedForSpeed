#include "checkpoint.h"

Checkpoint::Checkpoint(SDL2pp::Renderer& renderer, TextureManager& tm, const float x,
                       const float y):
        Entity(renderer, tm, x, y), frame(0) {}

// Crear archivo con todas estas funciones extras
void drawCircle(SDL2pp::Renderer& renderer, int cx, int cy, int radius) {
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
// _______________________________________________

void Checkpoint::draw(const Camera& camera) {
    constexpr int radius = 40;   // Por ahora

    int cx = static_cast<int>(x - camera.getX());
    int cy = static_cast<int>(y - camera.getY());

    if (active) {
        renderer.SetDrawColor(255, 255, 0, 180);
    } else {
        renderer.SetDrawColor(120, 120, 120, 180);
    }

    drawCircle(renderer, cx, cy, radius);

    if (!active) return;

    // Dibujá la bandera normalmente
    SDL2pp::Texture& texture(tm.getCities().getCheckpointTexture());
    const SDL2pp::Rect srcRect(tm.getCities().getCheckpointFrame(frame));

    frame = (frame + 1 > 40) ? 0 : frame + 1;

    int drawX = cx - 8;
    int drawY = cy - srcRect.h + 2;

    SDL2pp::Rect dstRect(drawX, drawY, srcRect.w, srcRect.h);

    renderer.Copy(texture, srcRect, dstRect);
}

void Checkpoint::setInactive() { active = false; }
