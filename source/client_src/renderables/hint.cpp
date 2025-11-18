#include "hint.h"


Hint::Hint(SDL2pp::Renderer& renderer, TextureManager& tm, const float x,
                       const float y):
        Entity(renderer, tm, x, y), angle(0), distance(0) {}
void Hint::draw(const Camera& camera, float cpX, float cpY) const {
    SDL2pp::Texture& tex = tm.getCities().getArrowTexture();

    int screenW = 1200;
    int screenH = 800;

    // Posición del auto en pantalla
    float carXscreen = x - camera.getX();
    float carYscreen = y - camera.getY();

    // Vector hacia el checkpoint
    float dx = cpX - x;
    float dy = cpY - y;

    // Ángulo hacia el checkpoint
    float angleRad = atan2(dy, dx);
    float angleDeg = angle + 90.0f;

    const float radius = 90.0f;
    float drawX = carXscreen + cos(angleRad) * radius;
    float drawY = carYscreen + sin(angleRad) * radius;

    const float margin = 35.0f;
    drawX = std::clamp(drawX, margin, screenW - margin);
    drawY = std::clamp(drawY, margin, screenH - margin);

    SDL_Rect dstRect = {
        int(drawX - tex.GetWidth() / 2),
        int(drawY - tex.GetHeight() / 2),
        tex.GetWidth(),
        tex.GetHeight()
    };

    renderer.Copy(
        tex,
        SDL_Rect{0, 0, tex.GetWidth(), tex.GetHeight()},
        dstRect,
        angleDeg,
        SDL2pp::Point(tex.GetWidth() / 2, tex.GetHeight() / 2)
    );
}

void Hint::update(const float newAngle, const float newDistance, const float carX, const float carY) {
    setAngle(newAngle);
    setDistance(newDistance);
    setPosition(carX, carY);
}


void Hint::setAngle(const float newAngle) {
    angle = fmod(newAngle, 2 * M_PI);
    if (angle < 0) angle += 2 * M_PI;
    angle = angle * 180.0 / M_PI;
}

void Hint::setDistance(const float newDistance) {
    distance = newDistance;
}


/*
 *
 *void Hint::draw(const Camera& camera, float cpX, float cpY) const {
    SDL2pp::Texture& tex = tm.getCities().getArrowTexture();

    // No se dibuja si está muy cerca
    const float disappearDistance = 150.0f;
    if (distance < disappearDistance) return;

    // Convertir posiciones a coordenadas de pantalla
    float carXscreen = x - camera.getX();
    float carYscreen = y - camera.getY();
    float cpXscreen  = cpX - camera.getX();
    float cpYscreen  = cpY - camera.getY();

    int screenW = 1200;
    int screenH = 800;

    // Usamos TU ángulo (ya está en grados)
    float angleDeg = angle;

    // Si el checkpoint está en pantalla → dibujarlo donde está
    if (cpXscreen >= 0 && cpXscreen <= screenW &&
        cpYscreen >= 0 && cpYscreen <= screenH) {

        SDL_Rect dstRect = {
            int(cpXscreen - tex.GetWidth()/2),
            int(cpYscreen - tex.GetHeight()/2),
            tex.GetWidth(),
            tex.GetHeight()
        };

        renderer.Copy(
            tex,
            SDL_Rect{0,0,tex.GetWidth(),tex.GetHeight()},
            dstRect,
            angleDeg,
            SDL2pp::Point(tex.GetWidth()/2, tex.GetHeight()/2)
        );
        return;
    }

    // Si está fuera → mover la flecha al borde correcto
    float dx = cpXscreen - carXscreen;
    float dy = cpYscreen - carYscreen;

    float t = 1.0f;

    if (dx != 0) {
        if (cpXscreen < 0) t = (0 - carXscreen) / dx;
        else if (cpXscreen > screenW) t = (screenW - carXscreen) / dx;
    }

    if (dy != 0) {
        float tY = 1.0f;
        if (cpYscreen < 0) tY = (0 - carYscreen) / dy;
        else if (cpYscreen > screenH) tY = (screenH - carYscreen) / dy;
        if (tY < t) t = tY;
    }

    float drawX = carXscreen + dx * t;
    float drawY = carYscreen + dy * t;

    // Margen hacia adentro
    const float margin = 35.0f;
    float rad = angleDeg * M_PI / 180.0f;
    drawX -= cos(rad) * margin;
    drawY -= sin(rad) * margin;

    SDL_Rect dstRect = {
        int(drawX - tex.GetWidth()/2),
        int(drawY - tex.GetHeight()/2),
        tex.GetWidth(),
        tex.GetHeight()
    };

    renderer.Copy(
        tex,
        SDL_Rect{0,0,tex.GetWidth(),tex.GetHeight()},
        dstRect,
        angleDeg,
        SDL2pp::Point(tex.GetWidth()/2, tex.GetHeight()/2)
    );
}*/