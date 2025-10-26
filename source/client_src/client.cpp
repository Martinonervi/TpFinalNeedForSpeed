#include "client.h"
#include <string>


Client::Client(const char* host, const char* service)
    : peer(host, service),
receiver(peer, queue), sender(peer)
{}


int jorge() {
    try {
        SDL2pp::SDL sdl(SDL_INIT_VIDEO);  // inicializar SDL primero

        SDL2pp::Window window("SDL2pp demo",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            640, 480,
            SDL_WINDOW_RESIZABLE);

        SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

        SDL2pp::Texture sprites(renderer, "../assets/cars/cars.png");

        bool running = true;
        SDL_Event event;
        while(running) {
            while(SDL_PollEvent(&event)) {
                if(event.type == SDL_QUIT) running = false;
            }

            renderer.SetDrawColor(0, 128, 0, 255);
            renderer.Clear();
            renderer.Copy(sprites);
            renderer.Present();
            SDL_Delay(16);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error SDL: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

void Client::Main() {
    sender.start();
    receiver.start();

    ClientWindow client_window(
        800,
        600,
        "Need For Speed",
        "../assets/cars/cars.png"
    );

    client_window.run();

    //jorge();
    stop();
    join();
}

void Client::stop(){
    sender.stop();
    receiver.stop();
}

void Client::join(){
    sender.join();
    receiver.join();
    close();
}
void Client::close() { peer.close(); }

