#include "tweeny.h"
#include <SFML/Graphics.hpp>
#include <vector>

struct AnimData {

    tweeny::tween<float, float> tween;
    sf::Sprite sprite;
    bool completed = false;
    std::function<void()>onComplete;

};

class TweenyAnimator {
public:

    void addMove(sf::Sprite sprite, sf::Vector2f start, sf::Vector2f end,
        int durationMs, std::function<void()>callback);
    void update(int dtMs);
    void render(sf::RenderWindow& window);
    bool isAnimating() const { return !m_tasks.empty(); }

private:

    std::vector<AnimData> m_tasks;
};