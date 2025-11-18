#include "hint.h"


Hint::Hint(SDL2pp::Renderer& renderer, TextureManager& tm, const float x,
                       const float y):
        Entity(renderer, tm, x, y), angle(0), distance(0) {}

void Hint::draw(const Camera& camera, float cpX, float cpY) const {
    SDL2pp::Texture& tex = tm.getCities().getArrowTexture();

    // Vector del auto al checkpoint
    double dx = cpX - x;
    double dy = cpY - y;
    double distance = sqrt(dx*dx + dy*dy);

    // Si está muy cerca, no dibujamos la flecha
    const float disappearDistance = 150.0f;
    if (distance < disappearDistance) return;

    // Ángulo en grados
    double angleDeg = atan2(dy, dx) * 180.0 / M_PI;

    // Tamaño de pantalla
    int screenW = 1200;
    int screenH = 800;

    // Punto de referencia inicial: borde de pantalla
    float drawX = x;
    float drawY = y;

    bool outOfScreen = cpX < 0 || cpX > screenW || cpY < 0 || cpY > screenH;
    if (outOfScreen) {
        // Línea desde el auto hasta el checkpoint
        float t = 1.0f;
        if (dx != 0) {
            if (cpX < 0) t = (0 - x) / dx;
            else if (cpX > screenW) t = (screenW - x) / dx;
        }
        if (dy != 0) {
            float tY = 1.0f;
            if (cpY < 0) tY = (0 - y) / dy;
            else if (cpY > screenH) tY = (screenH - y) / dy;
            if (tY < t) t = tY;
        }

        // Posición en borde
        float borderX = x + dx * t;
        float borderY = y + dy * t;

        // Interpolamos según distancia (más cerca = flecha más cerca del auto)
        const float maxDistance = 1000.0f; // distancia a la que la flecha empieza a alejarse al borde
        float factor = std::min(static_cast<float>(distance / maxDistance), 1.0f);
        drawX = borderX * factor + x * (1.0f - factor);
        drawY = borderY * factor + y * (1.0f - factor);
    } else {
        // Si el checkpoint está en pantalla, dibujar normal
        drawX = cpX;
        drawY = cpY;
    }

    // Rect de destino
    SDL_Rect dstRect = {
        static_cast<int>(drawX - tex.GetWidth()/2),
        static_cast<int>(drawY - tex.GetHeight()/2),
        tex.GetWidth(),
        tex.GetHeight()
    };
    SDL_Rect srcRect = {0, 0, tex.GetWidth(), tex.GetHeight()};

    renderer.Copy(
        tex,
        srcRect,
        dstRect,
        angleDeg,
        SDL2pp::Point(tex.GetWidth()/2, tex.GetHeight()/2)
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
