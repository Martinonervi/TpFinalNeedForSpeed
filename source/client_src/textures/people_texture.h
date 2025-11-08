#ifndef PEOPLE_TEXTURE_H
#define PEOPLE_TEXTURE_H

#include <SDL2pp/Texture.hh>
#include <SDL2pp/Rect.hh>
#include <map>

enum PersonType {
    PERSON_PLAYER,
    PERSON_NPC1,
    PERSON_NPC2
};

class PeopleTexture {
public:
    explicit PeopleTexture(SDL2pp::Texture& peopleTexture);
    SDL2pp::Rect getFrame(PersonType type) const;
    SDL2pp::Texture& getTexture() const;

private:
    SDL2pp::Texture& peopleTexture;
    struct PersonInfo { int xOffset; int yOffset; int width; int height; };
    std::map<PersonType, PersonInfo> personInfos;
};



#endif //PEOPLE_TEXTURE_H
