#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

class AudioManager {
private:
    std::unordered_map<std::string, sf::SoundBuffer> buffers;

    // pool sound đang chạy
    std::vector<sf::Sound> activeSounds;

    sf::Music backgroundMusic;

    float globalVolume = 100.f;

public:
    AudioManager();

    bool loadSound(const std::string& name, const std::string& file);

    void playSound(const std::string& name, float volume = 100.f, float pitch = 1.f);

    void update();

    bool playMusic(const std::string& file, bool loop = true);
    void stopMusic();

    void setVolume(float v);
    void initSound();
};