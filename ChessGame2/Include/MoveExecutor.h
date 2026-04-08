#pragma once
#include"MoveValidator.h"
#include"Define.h"


class MoveExecutor {
private: 
	
	Board& board;

	MoveType implementCastle(Move& move, Piece& piece);

	MoveType implementPromotion(Move& move, Piece& piece);

	MoveType implementEnPassant(Move& move, Piece& piece);

public:
	
	MoveExecutor(Board& b);

	void applyMove(Move& move);
	
};