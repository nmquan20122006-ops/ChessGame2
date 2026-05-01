#include"GameAudio.h"
#include"GameControl.h"

AudioManager::AudioManager(GameControl& g) :gameControl(g) {

    gameControl.subscribeToMove([this](const Move& move) {
        this->playSound("MoveSound", 100.f, 100.f);
        });
}

bool AudioManager::loadSound(const std::string& name, const std::string& filename) {

    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(filename)) {
        buffers[name] = buffer;
        return true;
    }
    return false;
}

void AudioManager::playSound(const std::string& name, float volume, float pitch) {

    if (buffers.find(name) != buffers.end()) {
        activeSounds.push_back(sf::Sound(buffers[name]));
        sf::Sound& s = activeSounds.back();

        s.setVolume(volume);
        s.setPitch(pitch);
        s.play();
       
    }
}

void AudioManager::update() {
    activeSounds.remove_if([](const sf::Sound& s) {
        return s.getStatus() == sf::Sound::Stopped;
        });
}

bool AudioManager::playMusic(const std::string& filename, bool loop) {
    if (!backgroundMusic.openFromFile(filename)) return false;
    backgroundMusic.setLoop(loop);
    backgroundMusic.play();
    return true;
}

void AudioManager::stopMusic() {
    backgroundMusic.stop();
}

void AudioManager::setMusicVolume(float volume) {
    backgroundMusic.setVolume(volume);
}

void AudioManager::initSound(){

    std::cout << "INIT SOUND CALLED\n";

    loadSound("MoveSound", "Assets/Sound/move.wav");
    loadSound("UnValidMoveSound", "Assets/Sound/illegalMove.wav");
    loadSound("CaptureSound", "Assets/Sound/Capture.wav");
    loadSound("CheckSound", "Assets/Sound/check.wav");
    loadSound("CheckMateSound", "Assets/Sound/checkmate.wav");
    loadSound("SelectSound", "Assets/Sound/select.wav");
}