#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <string>

#include <SDL2pp/SDL2pp.hh>

#include "../common_src/car.h"

class ClientWindow {
private:
    SDL2pp::SDL sdl;                 // Inicialización SDL
    SDL2pp::Window window;        // La ventana
    SDL2pp::Renderer renderer;    // El renderer
    SDL2pp::Texture carTexture;   // Textura de los autos
    Car playerCar;                    // Auto controlado por el jugador
    bool running;                     // Controla el bucle principal
    void handleEvents();              // Maneja input del teclado
    void render();                    // Dibuja todo

public:
    ClientWindow(int width, int height, const std::string& title, const std::string& carImagePath);

    void run();                       // Bucle principal

};

#endif // CLIENTWINDOW_H
