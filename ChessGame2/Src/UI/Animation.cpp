#include "Animation.h"
#include <algorithm>

void TweenyAnimator::addMove(sf::Sprite sprite, sf::Vector2f start,
    sf::Vector2f end, int durationMs, std::function<void()>callback) {
    AnimData task;
    task.sprite = sprite;
    task.sprite.setPosition(start); 
    task.onComplete = callback;

    task.tween = tweeny::from(start.x, start.y)
        .to(end.x, end.y)
        .during(durationMs)
        .via(tweeny::easing::quadraticOut);

    m_tasks.push_back(task);
}

void TweenyAnimator::update(int dtMs) {

    for (auto& task : m_tasks) {

        auto values = task.tween.step(dtMs);
        task.sprite.setPosition(values[0], values[1]);

        if (task.tween.progress() >= 0.999f) {
            task.completed = true;
            if (task.onComplete)task.onComplete();
        }
    }

    m_tasks.erase(
        std::remove_if(m_tasks.begin(), m_tasks.end(),
            [](const AnimData& t) { return t.completed; }),
        m_tasks.end()
    );
}

void TweenyAnimator::render(sf::RenderWindow& window) {
    for (const auto& task : m_tasks) {
        window.draw(task.sprite);
    }
}