#pragma once
#include"MoveService.h"
#include<random>
#include<vector>


class DumpBot {

private:

	Color botColor;

	BotMove botMove;

	GameState gameState;

	MoveService& moveService;

	std::mt19937 rng;

	std::vector<BotMove> bot_getAllValidMove(const Board& board);

public:

	DumpBot(MoveService& move, Color c);

	BotMove bot_makeMove(const Board& board);

	Color getBot_Color()const { return botColor; }
	void setBot_Color(Color c) { botColor = c; }
};