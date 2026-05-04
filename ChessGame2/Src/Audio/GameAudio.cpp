#include"GameAudio.h"
#include"GameControl.h"
#include"EventBus.h"

AudioManager::AudioManager() {

    auto& eventBus = EventBus::get();

    eventBus.subcribe(GameEvent::Capture,      [this] {playSound("CaptureSound"); });
    eventBus.subcribe(GameEvent::UnValidMove,  [this] {playSound("UnValidMoveSound"); });
    eventBus.subcribe(GameEvent::Move,         [this] {playSound("MoveSound"); });
    eventBus.subcribe(GameEvent::Check,        [this] {playSound("CheckSound"); });
    eventBus.subcribe(GameEvent::CheckMate,    [this] {playSound("CheckMateSound"); });
    eventBus.subcribe(GameEvent::Select,       [this] {playSound("SelectSund"); });
    eventBus.subcribe(GameEvent::Castle,       [this] {playSound("CastleSound"); });
    eventBus.subcribe(GameEvent::Promotion,    [this] {playSound("PromotionSound"); });

}

bool AudioManager::loadSound(const std::string& name, const std::string& file)
{
    sf::SoundBuffer buffer;

    if (!buffer.loadFromFile(file)) {
        std::cout << "Failed load: " << file << std::endl;
        return false;
    }

    buffers.emplace(name, std::move(buffer));
    return true;
}

void AudioManager::playSound(const std::string& name, float volume, float pitch)
{
    auto it = buffers.find(name);
    if (it == buffers.end()) return;

    // cleanup sound stopped trước
    activeSounds.erase(
        std::remove_if(activeSounds.begin(), activeSounds.end(),
            [](sf::Sound& s) {
                return s.getStatus() == sf::Sound::Stopped;
            }),
        activeSounds.end()
    );

    activeSounds.emplace_back();
    sf::Sound& sound = activeSounds.back();

    sound.setBuffer(it->second);
    sound.setVolume(volume * globalVolume / 100.f);
    sound.setPitch(pitch);
    sound.play();
}

void AudioManager::update()
{
    activeSounds.erase(
        std::remove_if(activeSounds.begin(), activeSounds.end(),
            [](const sf::Sound& s) {
                return s.getStatus() == sf::Sound::Stopped;
            }),
        activeSounds.end()
    );
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

void AudioManager::setVolume(float v)
{
    globalVolume = v;
    backgroundMusic.setVolume(v);
}

void AudioManager::initSound(){

    std::cout << "INIT SOUND CALLED\n";

    loadSound("MoveSound", "Assets/Sound/move.wav");
    loadSound("UnValidMoveSound", "Assets/Sound/illegalMove.wav");
    loadSound("CaptureSound", "Assets/Sound/Capture.wav");
    loadSound("CheckSound", "Assets/Sound/check.wav");
    loadSound("CheckMateSound", "Assets/Sound/checkmate.wav");
    loadSound("SelectSound", "Assets/Sound/select.wav");
    loadSound("CastleSound", SOUND_PATH + "castle.wav");
    loadSound("PromotionSound", SOUND_PATH + "promote.wav");
}