#pragma once

#include"Board.h"

class MoveValidator {

public:

	static bool isValidMove(Position fromPos, Position toPos, const Board& board);

	static bool checkPawn(Position fromPos, Position toPos, const Board& board);
	
	static bool checkRook(Position fromPos, Position toPos, const Board& board);

	static bool checkKnight(Position fromPos, Position toPos, const Board& board);

	static bool checkBishop(Position fromPos, Position toPos, const Board& board);

	static bool checkQueen(Position fromPos, Position toPos, const Board& board);

	static bool checkKing(Position fromPos, Position toPos, const Board& board);

	static bool isPathClear(Position fromPos, Position toPos, const Board& board);

	static bool isInCheck(bool whiteTurn,const Board& board);

	static bool isUnderAttack(Position pos, bool whiteTurn, const Board& board);

	static bool isCanEnPassant(Position fromPos, Position toPos, const Board& board);
	
	static bool isMoveLegal(Position fromPos, Position toPos, const Board& board);

	static bool isCanCastle(Position fromPos, Position toPos, const Board& board);

	static bool isCanExecuteMove(Position fromPos, Position toPos, const Board& board);

};