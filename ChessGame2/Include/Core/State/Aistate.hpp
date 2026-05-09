#pragma once
#include"Type.hpp"
#include<string>

struct EngineConfig {
	
	std::wstring		Path = L"stockfish.exe";
	int					difficulty = 20;
	int					thinkingMs = 2000;
	int					goDepth = 20;
	Color				turn = Color::black;
	bool				enabled = false;
};

enum class EngineThinkingMode { ENGINE_MOVE, HINT, IDLE };