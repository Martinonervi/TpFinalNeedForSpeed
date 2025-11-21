#ifndef MAP_H
#define MAP_H
#include "entity.h"
#include "../client_camera.h"


class Map final: public Entity {
public:
    Map(SDL2pp::Renderer& renderer, TextureManager& tm, MapType mapType);
    void draw(const Camera& camera) override;
    void drawOver(const Camera& camera);

private:
    MapType mapType;

};



#endif //MAP_H
