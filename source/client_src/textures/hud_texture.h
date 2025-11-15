#ifndef HUD_TEXTURE_H
#define HUD_TEXTURE_H
#include <SDL2pp/Texture.hh>

class HudTexture {
public:
    HudTexture(SDL2pp::Texture& bars, SDL2pp::Texture& checkpoint);
    SDL2pp::Rect getHealthBar() const;
    SDL2pp::Rect getEnergyBar() const;
    SDL2pp::Texture& getCheckpointTexture() const;
    SDL2pp::Texture& getBarsTexture() const;

private:
    SDL2pp::Texture& bars;
    SDL2pp::Texture& checkpoint;

};



#endif //HUD_TEXTURES_H
