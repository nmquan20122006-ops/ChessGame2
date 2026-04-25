#pragma once
#include"MoveService.h"
#include<random>
#include<vector>

class DumpBot {
private:

	color botColor;
	
	BotMove botMove;

	GameState gameState;

	MoveService& moveService;

	std::mt19937 rng;

	std::vector<BotMove> bot_getAllValidMove(const Board& board);

public:

	DumpBot(MoveService& move, color c);

	BotMove bot_makeMove(const Board& board);

	color getBot_color()const { return botColor; }
	void setBot_color(color c) { botColor = c; }
};