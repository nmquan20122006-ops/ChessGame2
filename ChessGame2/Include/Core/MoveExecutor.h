#pragma once
#include"MoveValidator.h"


class MoveExecutor {
private: 
	
	Board& board;

	MoveType moveType = MoveType::none;

	MoveType implementCastle(Move& move, Piece& piece);

	MoveType implementPromotion(Move& move, Piece& piece);

	MoveType implementEnPassant(Move& move, Piece& piece);


public:
	
	MoveExecutor(Board& b);

	void applyMove(Move& move);

	void undoCastle(const Move& move);
	
	void undoMove(const Move& move);
	void redoMove(const Move& move);
};