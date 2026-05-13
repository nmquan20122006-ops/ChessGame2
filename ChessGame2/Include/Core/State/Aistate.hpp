#pragma once
#include"Type.hpp"
#include<string>

struct EngineConfig {
	std::wstring		Path = L"stockfish.exe";
	int					difficulty = 20;
	int					thinkingMs = 2000;
	int					goDepth = 12;
	Color				turn = Color::black;
	bool				enabled = false;
};

struct EngineEvaluation {
	int					score = 0;
	int					checkMateIn = 0;
	bool				isCheckMate = false;
};

enum class EngineTaskQueue { move, hint, enemyMove, evaluation };

enum class EngineThinkingMode { ENGINE_MOVE, HINT, IDLE, ENEMY_BEST_MOVE_HINT };