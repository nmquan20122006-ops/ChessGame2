/*#pragma once
#include<string>
#include<iostream>
#include<random>

#include"Board.h"
#include"GameState.h"

class BoardToFEN_DEBUG {

public:

	BoardToFEN_DEBUG();

	Board board_Real;

	std::string turnToString(Color Color);
	std::string castleToString(Board& b);
	std::string enPassantToString(Move& move);

	char turnToChar(Color Color);
	char pieceToChar(Piece piece);

	void printBoard(Board& b);

	std::string toFEN(Board& b, GameState& state_Real,Move& move);
	
	Position getRandom();

	void runTest(Board& b, int numMoves);
	
};*/