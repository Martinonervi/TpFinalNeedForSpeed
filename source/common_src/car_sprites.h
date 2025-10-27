#ifndef CAR_SPRITES_H
#define CAR_SPRITES_H

#include <array>

#define SMALL_CAR 32
#define MEDIUM_CAR 40
#define LARGE_CAR 48

// Tira error con define
constexpr int FRAMES_PER_CAR = 16;
constexpr int FRAMES_PER_DIRECTION = 8;

struct SpriteFrame {
    int x;
    int y;
    int width;
    int height;
};

struct CarSprite {
    // Si uso string no funca el static constexpr
    const char* name;
    std::array<SpriteFrame, FRAMES_PER_CAR> frames;
};

enum CarType {
    CAR_GREEN,
    CAR_RED,
    CAR_PORSCHE,
    CAR_LIGHT_BLUE,
    CAR_JEEP,
    CAR_PICKUP,
    CAR_LIMO,
    CAR_COUNT
};

constexpr std::array<SpriteFrame, FRAMES_PER_CAR> make_frames(int y, int size) {
    std::array<SpriteFrame, FRAMES_PER_CAR> arr{};
    for (int i = 0; i < FRAMES_PER_DIRECTION; ++i) {
        arr[i] = { i * size, y, size, size };
        arr[i + FRAMES_PER_DIRECTION] = { i * size, y + size, size, size };
    }
    return arr;
}

// Segun tengo entendido al hacerlo de esta manera solo se genera una vez en el binario
static constexpr std::array<CarSprite, 7> CAR_SPRITES = {{
    { "Green", make_frames(0, SMALL_CAR) },
    { "Red", make_frames(64, MEDIUM_CAR) },
    { "Porsche", make_frames(144, MEDIUM_CAR) },
    { "Light Blue", make_frames(224, MEDIUM_CAR) },
    { "Jeep", make_frames(304, MEDIUM_CAR) },
    { "Pick up", make_frames(384, MEDIUM_CAR) },
    { "Limo", make_frames(464, LARGE_CAR) }
}};



#endif
