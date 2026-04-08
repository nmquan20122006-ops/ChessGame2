#pragma once
#include"AllMoveValid.h"
#include"MoveValidator.h"
#include"GameState.h"
#include"Define.h"


class MoveService {

private:

	AllValidMove generator;
	MoveValidator validator;

	bool isUnderAttack(Position pos, color target, const Board& board);

public:
	
	std::vector<Position>getValidMoves(Position fromPos, const Board& board) {
		return generator.getAllValidMoves(fromPos, board);
	}

	bool isValidMove(Position fromPos, Position toPos, const Board& board) {
		return validator.isCanExecuteMove(fromPos, toPos, board);
	}

	bool Check(const Board& board, color turn);
	bool CheckMate(const Board& board, color turn);

	Move createMove(Position from, Position to, Board& board);

};