/**
* @file MoveValidator.h
* @brief This file defines the MoveValidator class, which is responsible for validating the legality of moves in the chess game
* The MoveValidator class provides methods to check if a move is valid according to chess rules, including piece-specific movement rules, path clearance, check conditions, and special moves like castling and en passant.
* @author [Nguyen Minh Quan]
* @date [29/4/2026]
* @version [1.0]
* @copyright [Copyright (c) 2026 Nguyen Minh Quan. All rights reserved.]
* This game is developed for educational purposes and is not intended for commercial use.
* The code is provided "as is" without any warranties or guarantees of functionality, performance, or suitability for any particular purpose
* The author shall not be liable for any damages arising out of the use or inability to use the software, even if advised of the possibility of such damage.
*/

#pragma once
#include"Utility.h"
class Board;

class MoveValidator {

public:

	static bool checkPawn(Position fromPos, Position toPos, const Board& board);
	
	static bool checkRook(Position fromPos, Position toPos, const Board& board);

	static bool checkKnight(Position fromPos, Position toPos, const Board& board);

	static bool checkBishop(Position fromPos, Position toPos, const Board& board);

	static bool checkQueen(Position fromPos, Position toPos, const Board& board);

	static bool checkKing(Position fromPos, Position toPos, const Board& board);

	static bool isPathClear(Position fromPos, Position toPos, const Board& board);

	static bool isInCheck(bool whiteTurn,const Board& board);

	static bool isValidMove(Position fromPos, Position toPos, const Board& board);

	static bool isUnderAttack(Position pos, bool whiteTurn, const Board& board);

	static bool isCanEnPassant(Position fromPos, Position toPos, const Board& board);
	
	static bool isMoveLegal(Position fromPos, Position toPos, const Board& board);

	static bool isCanCastle(Position fromPos, Position toPos, const Board& board);

	static bool isCanExecuteMove(Position fromPos, Position toPos, const Board& board);

};