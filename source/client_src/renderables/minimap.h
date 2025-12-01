#ifndef MINIMAP_H
#define MINIMAP_H
#include <SDL2pp/SDL2pp.hh>

#include "../../common_src/constants.h"
#include "../../common_src/srv_msg/srv_recommended_path.h"
#include "../renderables/car.h"
#include "../textures/texture_manager.h"

class Minimap {
public:
    Minimap(MapType maptype, SDL2pp::Renderer& renderer, TextureManager& tm,
        std::vector<RecommendedPoint>& pathArray);
    void draw(int x, int y, std::unordered_map<ID, std::unique_ptr<Car>>& cars,
        ID playerId, int alphaMod, int mapSize) const;
    void drawRecommendedPath(int x, int y, float scaleX, float scaleY) const;

private:
    MapType maptype;
    SDL2pp::Renderer& renderer;
    TextureManager& tm;
    std::vector<RecommendedPoint>& pathArray;

};



#endif //MINIMAP_H
