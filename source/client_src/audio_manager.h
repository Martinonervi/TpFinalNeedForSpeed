#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL2pp/SDL2pp.hh>
#include <string>
#include <unordered_map>
#include "sdl_constants.h"

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void loadSound(const std::string& name, const std::string& path);
    void loadMusic(const std::string& name, const std::string& path);

    void playSound(const std::string& name, int loops = 0);
    void stopSound(const std::string& id);
    void playMusic(const std::string& name, int loops = -1);
    void stopMusic();

    void lowerVolume();
    void raiseVolume();

private:
    std::unordered_map<std::string, Mix_Chunk*> sounds;
    std::unordered_map<std::string, Mix_Music*> music;
    std::unordered_map<std::string, int> channels;

    int masterVolume = MIX_MAX_VOLUME/2;

    bool init();
    void initSfx();
    void close();
};

#endif
