#pragma once
#include<functional>
#include<vector>
#include<queue>
#include<unordered_map>

#include"State/Move.hpp"

class EventBus {
public:
	static EventBus& get() {
		static EventBus instance;
		return instance;
	}

	void publish(GameEvent event) {
		queue.push(event);
	}

	void subcribe(GameEvent event, std::function<void()>callback) {
		listeners[event].push_back(callback);
	}

	void dispatch();

private:
	EventBus() = default;
	std::queue<GameEvent> queue;
	std::unordered_map<GameEvent, std::vector<std::function<void()>>> listeners;
};