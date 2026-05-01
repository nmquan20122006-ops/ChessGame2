#pragma once
#include<vector>
#include"Utility.h"

class Board;

class AllValidMove {

public:

	static std::vector<Position>getAllValidMoves(Position fromPos, const Board& board);

	static std::vector<Position>getPawnAllMoves(Position fromPos, const Board& board);

	static std::vector<Position>getRookAllMoves(Position fromPos, const Board& board);

	static std::vector<Position>getKnightAllMoves(Position fromPos, const Board& board);

	static std::vector<Position>getBishopAllMoves(Position fromPos, const Board& board);

	static std::vector<Position>getQueenAllMoves(Position fromPos, const Board& board);

	static std::vector<Position>getRawMoves(Position fromPos, const Board& board);

	static std::vector<Position>getKingPhysicalMoves(Position fromPos, const Board& board);
};