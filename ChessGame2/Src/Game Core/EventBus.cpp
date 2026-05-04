#include"EventBus.h"

void EventBus::dispatch() {
	while (!queue.empty()) {
		auto e = queue.front(); queue.pop();
		auto it = listeners.find(e);
		if (it != listeners.end()) {
			for (auto& callback : it->second) callback();
		}
	}
}