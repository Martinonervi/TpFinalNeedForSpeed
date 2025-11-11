#ifndef MINIMAP_H
#define MINIMAP_H
#include "../../common_src/constants.h"
#include "../textures/texture_manager.h"
#include <SDL2pp/SDL2pp.hh>
#include "../renderables/car.h"

class Minimap {
public:
    Minimap(MapType maptype, SDL2pp::Renderer& renderer, TextureManager& tm);
    void draw(int x, int y, std::unordered_map<ID, std::unique_ptr<Car>>& cars) const;

private:
    MapType maptype;
    SDL2pp::Renderer& renderer;
    TextureManager& tm;
};



#endif //MINIMAP_H
