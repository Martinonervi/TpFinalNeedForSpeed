#include "checkpoint.h"

Checkpoint::Checkpoint(SDL2pp::Renderer& renderer, SdlDrawer& drawer, TextureManager& tm, const float x,
                       const float y):
        Entity(renderer, tm, x, y), frame(0), drawer(drawer) {}

void Checkpoint::draw(const Camera& camera) {
    constexpr int radius = 40;   // Por ahora

    int cx = static_cast<int>(x - camera.getX());
    int cy = static_cast<int>(y - camera.getY());

    if (active) {
        renderer.SetDrawColor(255, 255, 0, 180);
    } else {
        renderer.SetDrawColor(120, 120, 120, 180);
    }

    drawer.drawCircle(cx, cy, radius);

    if (!active) return;

    SDL2pp::Texture& texture(tm.getCities().getCheckpointTexture());
    const SDL2pp::Rect srcRect(tm.getCities().getCheckpointFrame(frame));

    frame = (frame + 1 > 40) ? 0 : frame + 1;

    const int drawX = cx - 8;
    const int drawY = cy - srcRect.h + 2;

    SDL2pp::Rect dstRect(drawX, drawY, srcRect.w, srcRect.h);

    renderer.Copy(texture, srcRect, dstRect);
}

void Checkpoint::setInactive() { active = false; }
