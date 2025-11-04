#include "people_texture.h"
PeopleTexture::PeopleTexture(SDL2pp::Texture& peopleTexture)
    : peopleTexture(peopleTexture) {
    personInfos[PERSON_PLAYER] = {0, 0, 32, 32};
    personInfos[PERSON_NPC1] = {32, 0, 32, 32};
    personInfos[PERSON_NPC2] = {64, 0, 32, 32};
}

SDL2pp::Rect PeopleTexture::getFrame(PersonType type) const {
    const auto& info = personInfos.at(type);
    return {info.xOffset, info.yOffset, info.width, info.height};
}

SDL2pp::Texture& PeopleTexture::getTexture() const {
    return peopleTexture;
}