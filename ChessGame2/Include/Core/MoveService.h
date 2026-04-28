/**
* @file MoveService.h
* @brief This file defines the MoveService class, which is responsible for handling all aspects of move generation, validation, and check, checkmate state
* Detection in the chess game. The MoveService class provides methods to generate valid moves for a given piece, validate the legality of a move, and determine if a player is in check or checkmate.
* @author [Nguyen Minh Quan]
* @date [29/4/2026]
* @version [1.0]
* @copyright [Copyright (c) 2026 Nguyen Minh Quan. All rights reserved.]
* This game is developed for educational purposes and is not intended for commercial use.
* The code is provided "as is" without any warranties or guarantees of functionality, performance, or suitability for any particular purpose
* The author shall not be liable for any damages arising out of the use or inability to use the software, even if advised of the possibility of such damage.
*/
#pragma once
#include"AllMoveValid.h"
#include"MoveValidator.h"
#include"GameState.h"

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