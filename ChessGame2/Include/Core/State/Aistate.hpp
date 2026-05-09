#pragma once
#include"Type.hpp"
#include<string>

struct EngineConfig {
	
	std::wstring		Path = L"stockfish.exe";
	int					difficulty = 20;
	int					thinkingMs = 2000;
	Color				turn = Color::black;
	bool				enabled = false;
};

