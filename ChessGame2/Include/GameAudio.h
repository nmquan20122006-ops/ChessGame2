#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <list>


class AudioManager {
private:
    std::map<std::string, sf::SoundBuffer> buffers;

    std::list<sf::Sound> activeSounds;

    sf::Music backgroundMusic;

public:
    bool loadSound(const std::string& name, const std::string& filename);

    void playSound(const std::string& name, float volume = 100.f, float pitch = 1.0f);

    bool playMusic(const std::string& filename, bool loop = true);
    void stopMusic();
    void setMusicVolume(float volume);

    void setSoundVolume(float volume);

    void update();
};