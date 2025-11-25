#include "audio_manager.h"
#include <iostream>

AudioManager::AudioManager() {
    init();
}

bool AudioManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer error: " << Mix_GetError() << std::endl;
        return false;
    }
    return true;
}

void AudioManager::close() {
    for (auto& pair : sounds) {
        Mix_FreeChunk(pair.second);
    }
    for (auto& pair : music) {
        Mix_FreeMusic(pair.second);
    }

    sounds.clear();
    music.clear();

    Mix_CloseAudio();
}

void AudioManager::loadSound(const std::string& name, const std::string& path) {
    Mix_Chunk* s = Mix_LoadWAV(path.c_str());
    if (!s) {
        std::cout << "Error cargando sonido '" << name << "': " << Mix_GetError() << std::endl;
        return;
    }
    sounds[name] = s;
}

void AudioManager::loadMusic(const std::string& name, const std::string& path) {
    Mix_Music* m = Mix_LoadMUS(path.c_str());
    if (!m) {
        std::cout << "Error cargando música '" << name << "': " << Mix_GetError() << std::endl;
        return;
    }
    music[name] = m;
}

void AudioManager::playSound(const std::string& name, int loops) {
    if (sounds.count(name)) {
        int ch = Mix_PlayChannel(-1, sounds[name], loops); // retornamos el canal
        if (ch != -1) {
            channels[name] = ch; // guardamos canal para stopSound
        }
    }
}

void AudioManager::stopSound(const std::string& name) {
    if (channels.count(name)) {
        Mix_HaltChannel(channels[name]); // detiene ese canal
        channels.erase(name);            // eliminamos registro
    }
}


void AudioManager::playMusic(const std::string& name, int loops) {
    if (music.count(name)) {
        Mix_PlayMusic(music[name], loops);
    }
}

void AudioManager::stopMusic() {
    Mix_HaltMusic();
}

AudioManager::~AudioManager() {
    close();
}