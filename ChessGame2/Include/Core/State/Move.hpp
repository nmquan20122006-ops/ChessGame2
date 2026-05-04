#pragma once
#include"Piece.h"
#include"Type.hpp"
#include"Utility.h"

struct Move {

	//Basic move data
	Position fromPos = { -1,-1 };
	Position toPos = { -1,-1 };

	Position enPassantCapturedPos = { -1,-1 };

	Piece movedPiece = Piece::Empty;
	Piece capturedPiece = Piece::Empty;
	Piece promotionPiece = Piece::Empty;

	MoveType moveType = MoveType::none;


	/**
	* @brief Resets the move data to default values Useful for clearing move information after processing a move.
	* NOTE: This does not reset the board state snapshots, as they may be needed for undo operations or move logging.
	*/
	void reset() {
		*this = Move(); // Reset all members to their default values
	}
};

// BotMove struct to represent a move made by the AI bot
struct BotMove {
	Position fromPos, toPos;
};

enum class GameEvent { Move, Capture, Select, Check, CheckMate, Promotion,
						UnValidMove, EnPassant, Castle };
