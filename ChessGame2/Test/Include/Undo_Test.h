/*#pragma once

#include "Board.h"
#include"GameState.h"
#include"BoardToFEN_Test.h"

#include<iostream>

class Board_DEBUG {

public:

	Board_DEBUG();

	std::vector<Move>undoStack = {};

	std::vector<Position>moveListTo;
	std::vector<Position>moveListFrom;

	Board board_Real;
	Move move_Real;
	GameState state_Real;

	void printBoard(Board& b);

	void printMoveList();

	void printUndoStack();

	void move_Test(Board& b, BoardToFEN_DEBUG& fenDebugger,Position from, Position to);

	void printMoveLog();

	void updateMoveLog(Position from, Position to, Piece capturedPiece, Piece movedPiece);

	void undo_Test(Board& b);

	void verifyAfterMove(Board& b, Position from, Position to, Piece movedPiece);

	void verifyAfterPushBack(Board& b, std::vector<Move>v);

	void runTest(Board& b, BoardToFEN_DEBUG& a, int num);
};
*/